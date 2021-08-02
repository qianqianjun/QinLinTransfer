#include <QDesktopServices>
#include <QDir>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QUrl>
#include "filetransferreceiver.h"
#include "settings.h"

FileTransferReceiver::FileTransferReceiver(QObject *parent, QTcpSocket *socket) :
    FileTransferSession(parent, socket),writingFile(nullptr),downloadPath(Settings::downloadPath()) {
}

void FileTransferReceiver::respond(bool accepted)
{
    QJsonObject obj;
    obj.insert("response", static_cast<int>(accepted));
    // 继承自filetransfersession
    // 将 accept发送
    encryptAndSend(QJsonDocument(obj).toJson(QJsonDocument::Compact)); // compact 参数会生成紧密的json格式（没有换行和空格）
    if (accepted) {
        if (!QDir().mkpath(downloadPath)) {
            emit errorOccurred(QString("没有权限创建下载目录：") + downloadPath);
            return;
        }
        if (!QFileInfo(downloadPath).isWritable()) {
            emit errorOccurred(QString("没有权限写入下载目录：") + downloadPath);
            return;
        }
        state = TRANSFERRING; // 继承自 filtersession
        createNextFile();
    } else {
        // 如果接收端拒绝接收
        connect(socket, &QTcpSocket::bytesWritten, this, &FileTransferSession::ended);
    }
}

void FileTransferReceiver::processReceivedData(const QByteArray &data)
{
    if (state == HANDSHAKE2) {
        QJsonDocument json = QJsonDocument::fromJson(data);
        if (!json.isObject()) {
            emit ended();
            return;
        }

        QJsonObject obj = json.object();
        QJsonValue deviceName = obj.value("device_name");
        if (!deviceName.isString()) {
            emit ended();
            return;
        }

        QJsonValue filesJson = obj.value("files");
        if (!filesJson.isArray()) {
            emit ended();
            return;
        }

        QJsonArray filesJsonArray = filesJson.toArray();
        if (filesJsonArray.empty()) {
            emit ended();
            return;
        }

        foreach (const QJsonValue &v, filesJsonArray) {
            if (!v.isObject()) {
                emit ended();
                return;
            }
            QJsonObject o = v.toObject();

            QJsonValue filename = o.value("filename");
            if (!filename.isString()) {
                emit ended();
                return;
            }

            QJsonValue size = o.value("size");
            if (!size.isDouble()) {
                emit ended();
                return;
            }

            quint64 sizeInt = static_cast<quint64>(size.toDouble());
            totalSize += sizeInt;
            transferQ.append({filename.toString(), sizeInt});
        }

        emit fileMetadataReady(transferQ, totalSize, deviceName.toString(),
                               crypto.sessionKeyDigest());
    } else if (state == TRANSFERRING) {
        transferredSize += data.size();
        emit updateProgress(static_cast<double>(transferredSize) / totalSize);
        QByteArray tmpData = data;
        while (tmpData.size() > 0) {
            FileMetadata &curFile = transferQ.first();
            quint64 writeSize = qMin(curFile.size, static_cast<quint64>(tmpData.size()));
            qint64 written = writingFile->write(tmpData.left(writeSize));
            curFile.size -= written;
            tmpData = tmpData.mid(written);
            if (curFile.size == 0) {
                transferQ.pop_front();
                createNextFile();
            }
        }
    }
}

void FileTransferReceiver::createNextFile()
{
    while (!transferQ.empty()) {
        FileMetadata &curFile = transferQ.first();
        QString filename = downloadPath + QDir::separator() + curFile.filename;
        if (writingFile) {
            // 删除对象，防止内存泄露。
            writingFile->deleteLater(); // 继承自 QObject。
            writingFile = nullptr;
        }
        writingFile = new QFile(filename, this);
        if (!writingFile->open(QIODevice::WriteOnly)) {
            emit errorOccurred(QString("无法发开文件： %1。").arg(filename));
            return;
        }
        if (curFile.size > 0) {
            emit printMessage(QString("接收文件： %1...").arg(curFile.filename));
            break;
        }
        transferQ.pop_front();
    }
    if (transferQ.empty()) {
        if (writingFile) {
            writingFile->deleteLater();
            writingFile = nullptr;
        }
        state = FINISHED;
        QDesktopServices::openUrl(QUrl::fromLocalFile(downloadPath));
        emit printMessage(QString("传输完成！"));
        socket->disconnectFromHost();
        QTimer::singleShot(5000, this, &FileTransferSession::ended); // 经过一段时间后执行 ended。
    }
}
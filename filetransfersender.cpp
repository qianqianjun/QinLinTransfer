#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>

#include "filetransfersender.h"
#include "settings.h"

FileTransferSender::FileTransferSender(QObject *parent, QTcpSocket *socket, const QList<QSharedPointer<QFile>> &files) :
    FileTransferSession(parent, socket), files(files){
    // 当有写入操作时，bytesWritten就会被激活。
    connect(socket, &QTcpSocket::bytesWritten, this, &FileTransferSender::socketBytesWritten);

    foreach (QSharedPointer<QFile> file, files) {
        QString filename = QFileInfo(*file).fileName();
        quint64 size = static_cast<quint64>(file->size());
        totalSize += size;
        transferQ.append({filename, size});
    }
}

void FileTransferSender::handshake1Finished()
{
    QJsonArray jsonFiles;
    foreach (FileMetadata metadata, transferQ) {
        QJsonObject jsonFile;
        jsonFile.insert("filename", metadata.filename);
        jsonFile.insert("size", static_cast<qint64>(metadata.size));
        jsonFiles.append(jsonFile);
    }

    QJsonObject obj;
    obj.insert("device_name", Settings::deviceName());
    obj.insert("device_type", QSysInfo::productType());
    obj.insert("files", jsonFiles);
    encryptAndSend(QJsonDocument(obj).toJson(QJsonDocument::Compact));
}

// 这里实际上是接收端发回来的确认信息，如果接收端同意接收文件，则发送端开始发送文件。
// 如果拒绝接收文件，则发送端不发送文件。
void FileTransferSender::processReceivedData(const QByteArray &data)
{
    if (state == HANDSHAKE2) {
        QJsonDocument json = QJsonDocument::fromJson(data);
        if (!json.isObject()) {
            emit errorOccurred(QString("连接失败！"));
            return;
        }

        QJsonObject obj = json.object();
        QJsonValue response = obj.value("response");
        if (!response.isDouble()) {
            emit errorOccurred(QString("连接失败！"));
            return;
        }

        if (response.toInt() == 0) {
            emit errorOccurred(QString("接收设备拒绝了连接请求！"));
            return;
        }
        state = TRANSFERRING;
        socketBytesWritten();
    }
}

void FileTransferSender::socketBytesWritten(){
    // socket->bytesToWrite() 返回等待写入的文件字节数
    // 当由于网络原因传输还没有完成时，该值大于0，此时不应该继续添加新的发送文件。
    if (state != TRANSFERRING || socket->bytesToWrite() > 0)
        return;

    while (!transferQ.empty()) {
        FileMetadata &curFile = transferQ.front();
        if (curFile.size == 0) {
            transferQ.pop_front();
            files.pop_front();
        } else {
            emit printMessage(QString("正在发送文件： %1...").arg(curFile.filename));
            break;
        }
    }
    if (transferQ.empty()) {
        state = FINISHED;
        emit printMessage(QString("传输完成！"));
        socket->disconnectFromHost();
        QTimer::singleShot(5000, this, &FileTransferSession::ended); // 调用ended
        return;
    }
    QSharedPointer<QFile> &curFile = files.front();
    FileMetadata &curMetadata = transferQ.front();
    QByteArray data = curFile->read(TRANSFER_QUANTA);
    encryptAndSend(data);
    curMetadata.size -= data.size();
    transferredSize += data.size();
    emit updateProgress(static_cast<double>(transferredSize) / totalSize);
}
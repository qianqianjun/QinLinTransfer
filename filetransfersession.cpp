#include <stdexcept>
#include "filetransfersession.h"

FileTransferSession::FileTransferSession(QObject *parent, QTcpSocket *socket) :
    QObject(parent), state(HANDSHAKE1), socket(socket), totalSize(0), transferredSize(0)
{
    qDebug()<<"FileTransferSession::FileTransferSession";
    socket->setParent(this);
    socket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    connect(socket, &QTcpSocket::readyRead, this, &FileTransferSession::socketReadyRead);
    // connect(socket,&QTcpSocket::errorOccurred,this, &FileTransferSession::socketErrorOccurred);
}

void FileTransferSession::start()
{
    qDebug()<<"FileTransferSession::start";
    emit printMessage(QString("握手中..."));
    // QTcpSocket 的write函数
    // write ( const char *, qint64 ) : qint64
    // write ( const char * ) : qint64
    // write ( const QByteArray & ) : qint64
    // write的意思是发送数据，这里发送的是公钥
    socket->write(crypto.localPublicKey());
}

void FileTransferSession::respond(bool) //
{
    qDebug()<<"FileTransferSession::respond";
    throw std::runtime_error("respond not implemented");
}

void FileTransferSession::encryptAndSend(const QByteArray &data)
{
    qDebug()<<"FileTransferSession::encryptAndSend";
    QByteArray sendData = crypto.encrypt(data);
    quint16 size = sendData.size();
    sendData.prepend(static_cast<quint8>(size & 0xFF)); // 将内容写到data的最前面
    sendData.prepend(static_cast<quint8>((size >> 8) & 0xFF));
    socket->write(sendData); // 发送数据
}

void FileTransferSession::handshake1Finished() {
    qDebug()<<"FileTransferSession handshake1Finished run";
}

void FileTransferSession::socketReadyRead()
{
    qDebug()<<"FileTransferSession::socketReadyRead";
    readBuffer += socket->readAll();

    if (state == HANDSHAKE1) {
        if (static_cast<quint64>(readBuffer.size()) < crypto.publicKeySize()) {
            emit errorOccurred(QString("握手失败！"));
            return;
        }
        QByteArray publicKey = readBuffer.left(crypto.publicKeySize());
        readBuffer = readBuffer.mid(crypto.publicKeySize());
        try {
            crypto.setRemotePublicKey(publicKey);
        } catch (const std::exception &e) {
            emit errorOccurred(e.what());
            return;
        }
        // emit printMessage(QString("建立连接，握手中: %1").arg(crypto.sessionKeyDigest()));
        state = HANDSHAKE2;
        qDebug()<<"要执行多态函数了！";
        handshake1Finished();
        qDebug()<<"多态函数执行完成！";
    }

    while (!readBuffer.isEmpty()) {
        if (readBuffer.size() < 2)
            break;

        quint16 size = static_cast<quint16>(static_cast<quint8>(readBuffer[0])) << 8;
        size |= static_cast<quint8>(readBuffer[1]);
        if (readBuffer.size() < size + 2)
            break;

        QByteArray data = readBuffer.mid(2, size);
        readBuffer = readBuffer.mid(size + 2);

        try {
            data = crypto.decrypt(data);
        } catch (const std::exception &e) {
            emit errorOccurred(e.what());
            return;
        }
        processReceivedData(data);
    }
}

void FileTransferSession::socketErrorOccurred()
{
    qDebug()<<"FileTransferSession::socketErrorOccurred";
    if (state != FINISHED)
        emit errorOccurred(socket->errorString());
}

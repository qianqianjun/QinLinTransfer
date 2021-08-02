#pragma once
#include <QObject>
#include <QTcpSocket>
#include "crypto.h"
class FileTransferSession : public QObject {
    Q_OBJECT
public:
    struct FileMetadata {
        QString filename;
        quint64 size;
    };
    explicit FileTransferSession(QObject *parent, QTcpSocket *socket);
    void start(); // 两个操作：（1）触发信号printMessage，显示正在握手，（2）发送公钥
    virtual void respond(bool accepted);
protected:
    enum State {
        HANDSHAKE1,
        HANDSHAKE2,
        TRANSFERRING,
        FINISHED
    } state;
    QTcpSocket *socket;
    Crypto crypto;
    QByteArray readBuffer;
    QList<FileMetadata> transferQ; // FileMetadata 包括文件名称和文件大小。
    quint64 totalSize;
    quint64 transferredSize;
    void encryptAndSend(const QByteArray &data);
    virtual void handshake1Finished();
    virtual void processReceivedData(const QByteArray &data) = 0;  // 纯虚函数，派生类中必须要实现。
private slots:
    void socketReadyRead();
    void socketErrorOccurred();
signals:
    void printMessage(const QString &msg);
    void updateProgress(double progress); // 更新比例
    void errorOccurred(const QString &msg);
    void fileMetadataReady(const QList<FileTransferSession::FileMetadata> &metadata, quint64 totalSize,
                           const QString &deviceName, const QString &sessionKeyDigest);
    void ended(); // 表示传送结束
};
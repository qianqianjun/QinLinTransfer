#pragma once

#include <QFile>

#include "filetransfersession.h"

class FileTransferSender : public FileTransferSession {
    Q_OBJECT
public:
    FileTransferSender(QObject *parent, QTcpSocket *socket, const QList<QSharedPointer<QFile>> &files);
private:
    enum {
        TRANSFER_QUANTA = 64000
    };
    QList<QSharedPointer<QFile>> files;
protected:
    void handshake1Finished();
    void processReceivedData(const QByteArray &data);
private slots:
    void socketBytesWritten();
};

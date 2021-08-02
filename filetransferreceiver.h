#pragma once

#include <QFile>

#include "filetransfersession.h"

class FileTransferReceiver : public FileTransferSession {
    Q_OBJECT
public:
    FileTransferReceiver(QObject *parent, QTcpSocket *socket);
    void respond(bool accepted);
protected:
    void processReceivedData(const QByteArray &data);
private:
    QFile *writingFile;
    QString downloadPath;
    void createNextFile();
};

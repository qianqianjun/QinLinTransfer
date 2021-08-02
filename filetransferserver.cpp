#include <stdexcept>
#include "filetransferdialog.h"
#include "filetransferreceiver.h"
#include "filetransferserver.h"
#include "settings.h"

FileTransferServer::FileTransferServer(QObject *parent) : QObject(parent) {}

void FileTransferServer::start(){
    quint16 port = Settings::serverPort();
    if (!server.listen(QHostAddress::Any, port))
        throw std::runtime_error(QString("无法监听 %1 端口").arg(port).toUtf8().toStdString());
    connect(&server, &QTcpServer::newConnection, this, &FileTransferServer::serverNewConnection);
}

quint16 FileTransferServer::port(){
    return server.serverPort();
}

/**
 * 每当有一个新的TCP连接，则执行下面的功能
 * 该功能由start函数设置。
 */
void FileTransferServer::serverNewConnection()
{
    while (server.hasPendingConnections()) { // 检查连接队列中是否有等待的连接。
        // 从等待队列中取出连接，（socket）
        FileTransferReceiver *receiver = new FileTransferReceiver(nullptr, server.nextPendingConnection());
        FileTransferDialog *d = new FileTransferDialog(nullptr, receiver);
        d->setAttribute(Qt::WA_DeleteOnClose);
    }
}
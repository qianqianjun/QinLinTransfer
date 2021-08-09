#include <QMessageBox>
#include <QPushButton>

#include "filetransferdialog.h"
#include "filetransfersender.h"
#include "sendtodialog.h"
#include "ui_sendtodialog.h"

SendToDialog::SendToDialog(QWidget *parent, const QList<QSharedPointer<QFile>> &files,
                           DiscoveryService &discoveryService) :
    QDialog(parent), ui(new Ui::SendToDialog), files(files)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    ui->hostsListView->setModel(&hostsStringListModel);
    connect(ui->hostsListView, &QListView::clicked, this, &SendToDialog::hostsListViewClicked);
    connect(ui->hostsListView, &QListView::doubleClicked, ui->buttonBox, &QDialogButtonBox::accepted);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(QString("发送"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(QString("取消"));

    connect(&discoveryService, &DiscoveryService::newHost, this, &SendToDialog::newHost);
    connect(&discoveryTimer, &QTimer::timeout, &discoveryService, &DiscoveryService::refresh);
    discoveryTimer.start(1000);
    discoveryService.refresh();

    connect(&socketTimeoutTimer, &QTimer::timeout, this, &SendToDialog::socketTimeout);
    socketTimeoutTimer.setSingleShot(true);
}

SendToDialog::~SendToDialog()
{
    delete ui;
}

void SendToDialog::newHost(const QString &deviceName, const QHostAddress &addr, quint16 port)
{
    // Endpoint: host 和 port的一个结构体
    QStringList l = hostsStringListModel.stringList();
    if (port == 0) {
        for (int i = 0; i < endpoints.size(); ++i) {
            if (endpoints[i].addr.isEqual(addr)) {
                endpoints.removeAt(i);
                l.removeAt(i);
                hostsStringListModel.setStringList(l);
                return;
            }
        }
        return;
    }
    for (int i = 0; i < endpoints.size(); ++i) {
        if (endpoints[i].addr.isEqual(addr)) {
            if (l.at(i) != deviceName) {
                l.replace(i, deviceName);
                hostsStringListModel.setStringList(l);
            }
            endpoints[i].port = port;
            return;
        }
    }
    endpoints.append({addr, port});
    l.append(deviceName);
    hostsStringListModel.setStringList(l);
}

void SendToDialog::hostsListViewClicked(const QModelIndex &index)
{
    Endpoint endpoint = endpoints[index.row()];
    bool isV4;
    quint32 ipv4 = endpoint.addr.toIPv4Address(&isV4);
    QString addr = isV4 ? QHostAddress(ipv4).toString() : endpoint.addr.toString();
    ui->addrLineEdit->setText(addr);
    ui->portLineEdit->setText(QString::number(endpoint.port));
}

void SendToDialog::accept()
{
    QString addr = ui->addrLineEdit->text();
    bool ok;
    quint16 port = ui->portLineEdit->text().toUShort(&ok);
    if (!ok) {
        QMessageBox::critical(this, QApplication::applicationName(),QString("端口号非法输入"));
        return;
    }
    // QTcpSocket
    socket = new QTcpSocket(this);
    connect(socket, &QTcpSocket::connected, this, &SendToDialog::socketConnected);
    connect(socket,
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
            &QTcpSocket::errorOccurred,
#else
            QOverload<QAbstractSocket::SocketError>::of(&QAbstractSocket::error),
#endif
            this, &SendToDialog::socketErrorOccurred);
    socket->connectToHost(addr, port);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    setCursor(QCursor(Qt::WaitCursor));
    // QTimer
    socketTimeoutTimer.start(5000);
}

void SendToDialog::socketConnected()
{
    socketTimeoutTimer.stop();
    FileTransferSender *sender = new FileTransferSender(nullptr, socket, files);
    FileTransferDialog *d = new FileTransferDialog(nullptr, sender);
    d->setAttribute(Qt::WA_DeleteOnClose);
    d->show();
    // 下面实际上是关闭sendtoDialog 窗口
    done(Accepted);
}

void SendToDialog::socketErrorOccurred()
{
    socketTimeoutTimer.stop();
    socket->disconnectFromHost();
    socket->close();
    socket->deleteLater();
    QMessageBox::critical(this, QApplication::applicationName(), socket->errorString());
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    setCursor(QCursor(Qt::ArrowCursor));
}

void SendToDialog::socketTimeout()
{
    socket->disconnectFromHost();
    socket->close();
    socket->deleteLater();
    QMessageBox::critical(this, QApplication::applicationName(), tr("Connection timed out"));
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    setCursor(QCursor(Qt::ArrowCursor));
}

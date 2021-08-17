#pragma once

#include <QObject>
#include <QUdpSocket>

class DiscoveryService : public QObject {
    Q_OBJECT
public:
    explicit DiscoveryService(QObject *parent = nullptr);
    void start(quint16 serverPort);
public slots:
    void refresh();
    void leave();
private:
    enum {
//        DISCOVERY_PORT=8081
        DISCOVERY_PORT = 52637
    };
    QUdpSocket socket;
    quint16 fileTransferPort;
    void sendInfo(const QHostAddress &addr, quint16 port);
    bool isLocalAddress(const QHostAddress &addr);
    QList<QHostAddress> broadcastAddresses();
private slots:
    void socketReadyRead();
signals:
    void newHost(const QString &deviceName, const QHostAddress &addr, quint16 port);
};

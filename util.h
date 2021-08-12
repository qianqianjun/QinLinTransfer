#ifndef UTIL_H
#define UTIL_H
#include <QString>
#include <QHostAddress>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QNetworkInterface>
typedef struct data{
    QString name;
    QString ip;
    quint16 port;
    QHostAddress addr;
    data(QString name,QString ip,qint16 port,QHostAddress addr):name(name),ip(ip),port(port),addr(addr){}
}DeviceInfo;

QString searchConfigFile(QString fileName);
QStringList getLocalHostIP();
class util
{
public:
    util();
};

#endif // UTIL_H

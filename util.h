#ifndef UTIL_H
#define UTIL_H
#include <QString>
#include <QHostAddress>
typedef struct data{
    QString name;
    QString ip;
    quint16 port;
    QHostAddress addr;
    data(QString name,QString ip,qint16 port,QHostAddress addr):name(name),ip(ip),port(port),addr(addr){}
}DeviceInfo;

class util
{
public:
    util();
};

#endif // UTIL_H

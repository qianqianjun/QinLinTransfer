#ifndef UTIL_H
#define UTIL_H
#include <QString>
#include <QHostAddress>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QNetworkInterface>
#include <QSettings>
#include <settings.h>
typedef struct data{
    QString name;
    QString ip;
    quint16 port;
    QHostAddress addr;
    data(QString name,QString ip,qint16 port,QHostAddress addr):name(name),ip(ip),port(port),addr(addr){}
}DeviceInfo;
typedef struct finfo{
    QString fileName;
    QString fileSize;
    finfo(QString name,QString size):fileName(name),fileSize(size){}
    finfo(){}
}FileInfo;
QString searchConfigFile(QString fileName);
QStringList getLocalHostIP();
QSettings* getSettings(QString group,QObject* parent);
class util
{
public:
    util();
};

typedef enum S {
    TRANSFER_KEY,
    TRANSFER_META,
    TRANSFER_DATA,
    TRANSFER_FINISH
}State;

typedef struct metadata{
    QString filename;
    quint64 size;
} FileMetadata;

class VirtualFile:public QObject{
public:
    explicit VirtualFile(VirtualFile* father,QObject* parent=nullptr);
    // 只有文件具有的字段
    FileInfo info;
    int key;

    // 只有目录具有的字段
    QString pwd;
    VirtualFile* father;
    QList<VirtualFile*> files;
    QMap<QString,VirtualFile*> dirs;

    void addFile(QList<QString> parts,int key,FileInfo fileinfo);
};

#endif // UTIL_H

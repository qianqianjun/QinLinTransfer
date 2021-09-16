#ifndef WEBCONTROLLER_H
#define WEBCONTROLLER_H
#include <QObject>
#include <util.h>
#include <httpserver/httplistener.h>
#include <httpserver/httprequesthandler.h>
#include <httpserver/staticfilecontroller.h>
#include <templateengine/templatecache.h>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>
#include <QJsonArray>

using namespace stefanfrings;
class Chunk:public QObject{
    Q_OBJECT
public:
    quint32 index;
    QString filename;
    QFile* filePtr;
    bool operator<(const Chunk &other) const{
        return index<other.index;
    }
    explicit Chunk(quint32 index,QString filename,QFile*& filePtr,QObject* parent=nullptr);
    ~Chunk();
};

class HostChunk:public QObject{
    Q_OBJECT
public:
    QTimer timer;
    QList<Chunk*> chunks;
    explicit HostChunk(QObject* parent=nullptr);
    ~HostChunk();
};
// Controller
class NotFoundController:public HttpRequestHandler{
    Q_OBJECT
private:
    TemplateCache* templateCache;
public:
    explicit NotFoundController(TemplateCache*& templateCache,QObject* parent=nullptr);
    void service(HttpRequest& request,HttpResponse& response);
};

class FileUploadController:public HttpRequestHandler{
    Q_OBJECT
private:
    TemplateCache* templateCache;
public:
    explicit FileUploadController(TemplateCache*& templateCache,QObject* parent=nullptr);
    void service(HttpRequest& request,HttpResponse& response);
};

class HugeFileUploadController:public HttpRequestHandler{
    Q_OBJECT
private:
    TemplateCache* templateCache;
    QMap<quint32,HostChunk*>* hostChunks;
public:
    explicit HugeFileUploadController(TemplateCache*& templateCache,
                                      QMap<quint32,HostChunk*>*& hostChunks,QObject* parent=nullptr);
    void service(HttpRequest& request,HttpResponse& response);
};

class FileListPageController:public HttpRequestHandler{
    Q_OBJECT
private:
    TemplateCache* templateCache;
public:
    explicit FileListPageController(TemplateCache*& templateCache,QObject* parent=nullptr);
    void service(HttpRequest& request,HttpResponse& response);
};

// 自定义的下载文件controller
class DECLSPEC DownloadFileController:public HttpRequestHandler{
    Q_OBJECT
    Q_DISABLE_COPY(DownloadFileController)
public:
    DownloadFileController(const QSettings* settings, QObject* parent = nullptr);
    void service(HttpRequest& request, HttpResponse& response);
private:
    QString encoding;
    QString docroot;
    int maxAge;
    struct CacheEntry {
        QByteArray document;
        qint64 created;
        QByteArray filename;
    };
    int cacheTimeout;
    int maxCachedFileSize;
    QCache<QString,CacheEntry> cache;
    QMutex mutex;
    void setContentType(const QString file, HttpResponse &response) const;
};

#endif // WEBCONTROLLER_H

#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <webcontroller.h>

// RequestMapper
class RequestMapper:public HttpRequestHandler{
    Q_OBJECT
private:
    // 模板的设置
    TemplateCache* templateCache;
    StaticFileController* staticFileController; // 静态文件路由设置
    DownloadFileController* downloadFileController; // 下载文件路由设置
    QMap<quint32,HostChunk*> *hostChunks;
public:
    explicit RequestMapper(QObject* parent=nullptr);
    void service(HttpRequest& request,HttpResponse& response);
    void setdownloadFileController(DownloadFileController*& controller);
};

class WebServer:public QObject
{
    Q_OBJECT
public:
    explicit WebServer(QObject* parent=nullptr);
    QString openSender(QString ip,qint16 port,QStringList filePath);
    QString openReceiver(QString ip,qint16 port);
    ~WebServer();
    // component
    RequestMapper* mapper;
    HttpListener* listener;
};

#endif // WEBSERVER_H

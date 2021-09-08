#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <QObject>
#include <util.h>
#include <httpserver/httplistener.h>
#include <httpserver/httprequesthandler.h>
#include <httpserver/staticfilecontroller.h>
#include <templateengine/templatecache.h>
#include <QApplication>
#include <QDateTime>
#include <QUrl>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDesktopServices>

using namespace stefanfrings;

// Controller
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
public:
    explicit HugeFileUploadController(TemplateCache*& templateCache,QObject* parent=nullptr);
    void service(HttpRequest& request,HttpResponse& response);
};


// RequestMapper
class RequestMapper:public HttpRequestHandler{
    Q_OBJECT
private:
    // 模板的设置
    TemplateCache* templateCache;
    StaticFileController* staticFileController; // 静态文件路由设置
    StaticFileController* downloadFileController; // 下载文件路由设置
public:
    explicit RequestMapper(QObject* parent=nullptr);
    void service(HttpRequest& request,HttpResponse& response);
    void setdownloadFileController(StaticFileController*& controller);
};


class WebServer:public QObject
{
    Q_OBJECT
public:
    explicit WebServer(QObject* parent=nullptr);
    QString openSender(QString ip,qint16 port,QString filePath);
    QString openReceiver(QString ip,qint16 port);
    ~WebServer();
    // component
    RequestMapper* mapper;
    HttpListener* listener;
};

#endif // WEBSERVER_H

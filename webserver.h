#ifndef WEBSERVER_H
#define WEBSERVER_H
#include <QObject>
#include <util.h>
#include <httpserver/httplistener.h>
#include <httpserver/httprequesthandler.h>
#include <httpserver/staticfilecontroller.h>
#include <templateengine/templatecache.h>
#include <QApplication>
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


// RequestMapper
class RequestMapper:public HttpRequestHandler{
    Q_OBJECT
private:
    TemplateCache* templateCache;
    StaticFileController* staticFileController;
public:
    explicit RequestMapper(QObject* parent=nullptr);
    void service(HttpRequest& request,HttpResponse& response);
};


class WebServer:public QObject
{
public:
    explicit WebServer(QObject* parent=nullptr);
    void openSender();
    void openReceiver();
    ~WebServer();
    // component
    RequestMapper* mapper;
    HttpListener* listener;
};

#endif // WEBSERVER_H

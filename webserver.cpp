#include "webserver.h"


RequestMapper::RequestMapper(QString confFile,QObject *parent):HttpRequestHandler(parent){
    QSettings* templeteSettings=new QSettings(confFile,QSettings::IniFormat,this);
    templeteSettings->beginGroup("templates");
    templateCache=new TemplateCache(templeteSettings,this);
    QSettings* fileSettings=new QSettings(confFile,QSettings::IniFormat,this);
    fileSettings->beginGroup("static");
    staticFileController=new StaticFileController(fileSettings,this);
}

void RequestMapper::setStaicFileController(StaticFileController*& controller){
    delete staticFileController;
    staticFileController=controller;
}
void RequestMapper::service(HttpRequest &request, HttpResponse &response){
    QByteArray path=request.getPath();
    if (path.startsWith("/file")){
        FileUploadController(templateCache).service(request, response);
    }else{
        staticFileController->service(request, response);
    }
}

FileUploadController::FileUploadController(TemplateCache*& templateCache, QObject* parent):HttpRequestHandler(parent),templateCache(templateCache){

}

void FileUploadController::service(HttpRequest &request, HttpResponse &response)
{
    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    Template t=templateCache->getTemplate("upload",request.getHeader("Accept-Language"));
    t.enableWarnings();
    if (request.getParameter("action")=="show")
    {
        QTemporaryFile* file=request.getUploadedFile("data");
        if (file){
            long totalSize=0;
            while (!file->atEnd() && !file->error())
            {
                QByteArray buffer=file->read(65536);
                totalSize+=buffer.size();
            }
            response.write(QString::number(totalSize).toUtf8());
        }else{
            response.write("-1");
        }
    }
    else{
        t.setCondition("finish",false);
        response.write(t.toUtf8(),true);
    }
}


WebServer::WebServer(QString fileName,QObject *parent):QObject(parent),fileName(fileName)
{
    confFile=searchConfigFile(fileName);
    // request mapper
    mapper=new RequestMapper(confFile,this);
    // listener
    QSettings* listenSetings=new QSettings(confFile,QSettings::IniFormat,this);
    listenSetings->beginGroup("listener");
    listener=new HttpListener(listenSetings,mapper,this);
}

QString WebServer::openSender(QString ip,qint16 port,QString filePath)
{
    QFileInfo fileInfo(filePath);
    QString staticPath=fileInfo.absolutePath();
    QString resource=fileInfo.fileName();

    QSettings* fileSettings=new QSettings(confFile,QSettings::IniFormat,this);
    fileSettings->beginGroup("static");
    fileSettings->setValue("path",staticPath);
    StaticFileController* staticFileController=new StaticFileController(fileSettings,this);
    mapper->setStaicFileController(staticFileController);
    return QString("http://%1:%2/%3").arg(ip).arg(port).arg(resource);
}

QString WebServer::openReceiver(QString ip,qint16 port)
{
    return QString("http://%1:%2/file").arg(ip).arg(port);
}

WebServer::~WebServer()
{
    delete mapper;
    delete listener;
}

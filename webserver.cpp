#include "webserver.h"


RequestMapper::RequestMapper(QObject *parent):HttpRequestHandler(parent){
    QString fileName("qilintransfer.ini");
    QString confFile=searchConfigFile(fileName);

    QSettings* templeteSettings=new QSettings(confFile,QSettings::IniFormat,this);
    templeteSettings->beginGroup("templates");
    templateCache=new TemplateCache(templeteSettings,this);


    QSettings* fileSettings=new QSettings(confFile,QSettings::IniFormat,this);
    fileSettings->beginGroup("static");
    fileSettings->setValue("path","/home/qianqianjun/Downloads");
    staticFileController=new StaticFileController(fileSettings,this);
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

WebServer::WebServer(QObject *parent):QObject(parent){

}

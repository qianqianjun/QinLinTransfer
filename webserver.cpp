#include "webserver.h"


RequestMapper::RequestMapper(QObject *parent):HttpRequestHandler(parent){
    QSettings* templeteSettings=getSettings("templates",this);
    templateCache=new TemplateCache(templeteSettings,this);
    QSettings* fileSettings=getSettings("static",this);
    staticFileController=new StaticFileController(fileSettings,this);
}

void RequestMapper::setdownloadFileController(StaticFileController*& controller){
    if(downloadFileController!=nullptr)
        delete downloadFileController;
    downloadFileController=controller;
}
void RequestMapper::service(HttpRequest &request, HttpResponse &response){
    QByteArray path=request.getPath();
    if (path.startsWith("/file")){
        FileUploadController(templateCache).service(request, response);
    }else if(path.startsWith("/hugefile")){
        HugeFileUploadController(templateCache).service(request,response);
    }else if(path.startsWith("/static")){
        staticFileController->service(request,response);
    }else if(path.startsWith("download")){
        // 这里的downloadFileController 没有初始化
        downloadFileController->service(request, response);
    }else{
        // 这里设置返回404界面
    }
}

HugeFileUploadController::HugeFileUploadController(
        TemplateCache *&templateCache, QObject *parent):HttpRequestHandler(parent),templateCache(templateCache){}

void HugeFileUploadController::service(HttpRequest &request, HttpResponse &response)
{
    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    Template t=templateCache->getTemplate("uploadhuge",request.getHeader("Accept-Language"));
    t.enableWarnings();
    if(request.getParameter("action")=="show"){
        QHostAddress addr=request.getPeerAddress();
        quint32 ip=addr.toIPv4Address();
        // 为每一个ip地址开启一个队列。
        // 每一次传送的数据 {ip,块数量,块数据}。
        // 如果一个文件传输完成，则排序该ip对应的数据块，合并数据块成文件，保存到主机上。
        // 删除该ip对应的队列。
    }else{
        response.write(t.toUtf8(),true);
    }
}



FileUploadController::FileUploadController(TemplateCache*& templateCache, QObject* parent):HttpRequestHandler(parent),templateCache(templateCache){}

void FileUploadController::service(HttpRequest &request, HttpResponse &response)
{
    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    Template t=templateCache->getTemplate("upload",request.getHeader("Accept-Language"));
    t.enableWarnings();
    if (request.getParameter("action")=="show") //
    {
        QString finish=request.getParameter("finish");
        if(finish=="finish"){
            QDesktopServices::openUrl(QUrl::fromLocalFile(Settings::downloadPath()));
            QJsonObject responseText;
            responseText.insert("ok", true);
            responseText.insert("msg", QString("文件已保存在%1%2").arg(Settings::downloadPath()).arg("！"));
            response.write(QJsonDocument(responseText).toJson(QJsonDocument::Compact));
            return;
        }
        QTemporaryFile* file=request.getUploadedFile("data");
        QString fileName=request.getParameter("filename");
        if (file){
            QString timestrap=QDateTime::currentDateTime().toString("yyyy-MM-dd-hh.mm.ss_");
            QString storeName=Settings::downloadPath()+QDir::separator() + timestrap+fileName;
            QFile* storeFile=new QFile(storeName,this);
            if(storeFile->open(QIODevice::WriteOnly)){
                while (!file->atEnd() && !file->error()){
                    QByteArray buffer=file->read(65536);
                    storeFile->write(buffer);
                }
            }
            storeFile->close();
            storeFile->deleteLater();
            QJsonObject responseText;
            responseText.insert("ok", true);
            responseText.insert("msg", QString("文件已保存在%1%2").arg(storeName).arg("！"));
            response.write(QJsonDocument(responseText).toJson(QJsonDocument::Compact));
        }else{
            QJsonObject responseText;
            responseText.insert("ok", false);
            responseText.insert("msg","文件上传失败！");
            response.write(QJsonDocument(responseText).toJson(QJsonDocument::Compact));
        }
    }
    else{
        response.write(t.toUtf8(),true);
    }
}

WebServer::WebServer(QObject *parent):QObject(parent)
{
    // request mapper
    mapper=new RequestMapper(this);
    // listener
    QSettings* listenSetings=getSettings("listener",this);
    listener=new HttpListener(listenSetings,mapper,this);
}

QString WebServer::openSender(QString ip,qint16 port,QString filePath)
{
    QFileInfo fileInfo(filePath);
    QString staticPath=fileInfo.absolutePath();
    QString resource=fileInfo.fileName();

    QSettings* fileSettings=getSettings("static",this);
    fileSettings->setValue("path",staticPath);
    StaticFileController* downloadFileController=new StaticFileController(fileSettings,this);
    mapper->setdownloadFileController(downloadFileController);
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

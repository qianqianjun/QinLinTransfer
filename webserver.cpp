#include "webserver.h"

RequestMapper::RequestMapper(QObject *parent):HttpRequestHandler(parent){
    QSettings* templeteSettings=getSettings("templates",this);
    templateCache=new TemplateCache(templeteSettings,this);
    QSettings* fileSettings=getSettings("static",this);
    staticFileController=new StaticFileController(fileSettings,this);
    hostChunks=new QMap<quint32,HostChunk*>();
    downloadFileController=nullptr;
}

void RequestMapper::setdownloadFileController(DownloadFileController*& controller){
    if(downloadFileController!=nullptr){
        delete downloadFileController;
        downloadFileController=nullptr;
    }
    downloadFileController=controller;
}

void RequestMapper::service(HttpRequest &request, HttpResponse &response){
    QByteArray path=request.getPath();
    if (path.startsWith("/file")){
        FileUploadController(templateCache).service(request, response);
    }else if(path.startsWith("/hugefile")){
        HugeFileUploadController(templateCache,hostChunks).service(request,response);
    }else if(path.startsWith("/static")){
        staticFileController->service(request,response);
    }else if(path.startsWith("/download")){
        downloadFileController->service(request, response);
    }else if(path.startsWith("/list")){
        FileListPageController(templateCache).service(request,response);
    }else{
        NotFoundController(templateCache).service(request,response);
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

QString WebServer::openSender(QString ip,qint16 port,QStringList filePaths)
{
    QList<QFileInfo> fileInfos;
    for(int i=0;i<filePaths.size();i++)
        fileInfos.append(QFileInfo(filePaths[i]));
    // 下面要设置request的请求参数: JSON
    QJsonArray fileArray;
    foreach(QFileInfo info,fileInfos){
        QJsonObject file;
        file.insert("size",info.size());
        file.insert("filename",info.fileName());
        fileArray.append(file);
    }
    // 下面要设置静态文件的地址
    QString staticPath=fileInfos[0].absolutePath();
    QSettings* fileSettings=getSettings("static",this);
    fileSettings->setValue("path",staticPath);

    QString resource=QJsonDocument(fileArray).toJson(QJsonDocument::Compact);
    DownloadFileController* downloadFileController=new DownloadFileController(fileSettings,this);
    mapper->setdownloadFileController(downloadFileController);
    return QString("http://%1:%2/list?files=%3").arg(ip).arg(port).arg(resource);
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

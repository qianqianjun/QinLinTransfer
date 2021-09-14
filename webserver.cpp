#include "webserver.h"

RequestMapper::RequestMapper(QObject *parent):HttpRequestHandler(parent){
    QSettings* templeteSettings=getSettings("templates",this);
    templateCache=new TemplateCache(templeteSettings,this);
    QSettings* fileSettings=getSettings("static",this);
    staticFileController=new StaticFileController(fileSettings,this);
    hostChunks=new QMap<quint32,HostChunk*>();
}

Chunk::Chunk(quint32 index, QString filename, QFile*& filePtr, QObject *parent):
    QObject(parent),index(index),filename(filename),filePtr(filePtr){}

Chunk::~Chunk(){
    // 删除块指针所指向的块文件，关闭文件，删除块指针
    filePtr->remove();
    filePtr->close();
    filePtr->deleteLater();
}

HostChunk::HostChunk(QObject *parent):QObject(parent){}

HostChunk::~HostChunk(){
    for(int i=0;i<chunks.size();i++){
        chunks[i]->deleteLater();
    }
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
        HugeFileUploadController(templateCache,hostChunks).service(request,response);
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
        TemplateCache *&templateCache,QMap<quint32,HostChunk*>*& hostChunks,
        QObject *parent):HttpRequestHandler(parent),templateCache(templateCache),
        hostChunks(hostChunks){
}

void HugeFileUploadController::service(HttpRequest &request, HttpResponse &response)
{
    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    Template t=templateCache->getTemplate("uploadhuge",request.getHeader("Accept-Language"));
    t.enableWarnings();
    if(request.getParameter("action")=="show"){
        QHostAddress addr=request.getPeerAddress();
        quint32 ip=addr.toIPv4Address();
        if(request.getParameter("begin")=="begin"){
            if(hostChunks->contains(ip)){
                // 只有在遇到故障的时候才会执行这里。
                hostChunks->value(ip)->deleteLater();
            }
            hostChunks->insert(ip,new HostChunk(nullptr));
            QJsonObject responseText;
            responseText.insert("ok", true);
            responseText.insert("msg","大文件传输初始化成功！");
            response.write(QJsonDocument(responseText).toJson(QJsonDocument::Compact));
            return;
        }
        if(request.getParameter("finish")=="finish"){
            QString fileName=request.getParameter("filename");
            // 合并文件，完成最后的操作。
            QList<Chunk*> blocks=hostChunks->value(ip)->chunks;
            QString timestrap=QDateTime::currentDateTime().toString("yyyy-MM-dd-hh.mm.ss_");
            QString storeName=Settings::downloadPath()+QDir::separator()+timestrap+fileName;
            QFile* storeFile=new QFile(storeName,this);
            if(storeFile->open(QIODevice::ReadWrite)){
                for(int i=0;i<blocks.size();i++){
                    blocks[i]->filePtr->seek(0);
                    storeFile->write(blocks[i]->filePtr->readAll());
                }
                storeFile->close();
                storeFile->deleteLater();

                hostChunks->value(ip)->deleteLater();
                hostChunks->remove(ip);

                QDesktopServices::openUrl(QUrl::fromLocalFile(Settings::downloadPath()));
                QJsonObject responseText;
                responseText.insert("ok", true);
                responseText.insert("msg", QString("文件已保存在%1%2").arg(Settings::downloadPath()).arg("！"));
                response.write(QJsonDocument(responseText).toJson(QJsonDocument::Compact));
            }else{
                // 文件打开失败
                hostChunks->value(ip)->deleteLater();
                hostChunks->remove(ip);
                QJsonObject responseText;
                responseText.insert("ok", false);
                responseText.insert("msg","文件块合并失败，文件无法打开，请检查是否具有目录权限！");
                response.write(QJsonDocument(responseText).toJson(QJsonDocument::Compact));
            }
            return;
        }
        if(request.getParameter("finish")=="terminate"){
            // 出现错误，删除已经保存的块数据
            hostChunks->value(ip)->deleteLater();
            hostChunks->remove(ip);
            QJsonObject responseText;
            responseText.insert("ok", true);
            responseText.insert("msg","删除失败的任务成功！");
            response.write(QJsonDocument(responseText).toJson(QJsonDocument::Compact));
            return;
        }
        // 每一次传送的数据 {ip,index,块数据}。
        QTemporaryFile* file=request.getUploadedFile("data");
        QString fileName=request.getParameter("filename");
        QString index=request.getParameter("index");
        if (file){
            QString storeName=Settings::downloadPath()+QDir::separator()+fileName+index;
            QFile* storeFile=new QFile(storeName,nullptr);
            if(storeFile->open(QIODevice::ReadWrite)){
                while (!file->atEnd() && !file->error()){
                    QByteArray buffer=file->readAll();
                    storeFile->write(buffer);
                }
                // 增加新的chunk块
                hostChunks->value(ip)->chunks.append(new Chunk(index.toUInt(),fileName,storeFile));
                QJsonObject responseText;
                responseText.insert("ok", true);
                response.write(QJsonDocument(responseText).toJson(QJsonDocument::Compact));
            }else{
                // 文件打开失败
                QJsonObject responseText;
                responseText.insert("ok", false);
                responseText.insert("msg","保存块数据失败，文件无法打开，请检查是否具有目录权限！");
                response.write(QJsonDocument(responseText).toJson(QJsonDocument::Compact));
            }
        }else{
            QJsonObject responseText;
            responseText.insert("ok", false);
            responseText.insert("msg","读取上传的文件失败！");
            response.write(QJsonDocument(responseText).toJson(QJsonDocument::Compact));
        }
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
                    QByteArray buffer=file->readAll();
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

#include "webcontroller.h"
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
            QString storeName=Settings::downloadPath()+"/"+fileName;
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

NotFoundController::NotFoundController(
        TemplateCache *&templateCache,
        QObject *parent):HttpRequestHandler(parent),templateCache(templateCache){}

void NotFoundController::service(
        HttpRequest &request,
        HttpResponse &response){
    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    Template t=templateCache->getTemplate("404",request.getHeader("Accept-Language"));
    t.enableWarnings();
    response.write(t.toUtf8(),true);
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
            QString storeName=Settings::downloadPath()+"/"+ fileName;
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

FileListPageController::FileListPageController(TemplateCache *&templateCache, QObject *parent):
    HttpRequestHandler(parent),templateCache(templateCache){}
QString parseSize(quint64 size){
    QStringList units;
    units<<"B"<<"KB"<<"MB"<<"GB"<<"TB";
    int i=0;
    double capacity=size;
    while(capacity>1000){
        i++;
        capacity/=1000;
    }
    return QString("%1%2").arg(QString::number(capacity,'g',2)).arg(units[i]);
}
void FileListPageController::service(HttpRequest &request, HttpResponse &response){
    response.setHeader("Content-Type", "text/html; charset=UTF-8");
    Template t=templateCache->getTemplate("files",request.getHeader("Accept-Language"));
    t.enableWarnings();
    QByteArray jsonStr=request.getParameter("files");
    QJsonDocument json = QJsonDocument::fromJson(jsonStr);
    QJsonArray jsonArray = json.array();
    QList<QPair<QString,quint64>> filelist;
    foreach(const QJsonValue &file,jsonArray){
        QJsonObject fileObj=file.toObject();
        QJsonValue filename=fileObj.value("filename");
        QJsonValue size=fileObj.value("size");
        filelist.append(QPair<QString,quint64>{filename.toString(),static_cast<quint64>(size.toInt())});
    }
    // 将数据传递到前端界面
    t.loop("row",filelist.size());
    for(int i=0;i<filelist.size();i++){
        QString number=QString::number(i);
        QString name=filelist[i].first;
        quint64 size=filelist[i].second;
        t.setVariable("row"+number+".name",name);
        t.setVariable("row"+number+".size",parseSize(size));
    }
    response.write(t.toUtf8(),true);
}


DownloadFileController::DownloadFileController(const QSettings *settings, QObject* parent)
    :HttpRequestHandler(parent)
{
    maxAge=settings->value("maxAge","60000").toInt();
    encoding=settings->value("encoding","UTF-8").toString();
    docroot=settings->value("path",".").toString();
    if(!(docroot.startsWith(":/") || docroot.startsWith("qrc://")))
    {
        // Convert relative path to absolute, based on the directory of the config file.
        #ifdef Q_OS_WIN32
            if (QDir::isRelativePath(docroot) && settings->format()!=QSettings::NativeFormat)
        #else
            if (QDir::isRelativePath(docroot))
        #endif
        {
            QFileInfo configFile(settings->fileName());
            docroot=QFileInfo(configFile.absolutePath(),docroot).absoluteFilePath();
        }
    }
    qDebug("DownloadFileController: docroot=%s, encoding=%s, maxAge=%i",qPrintable(docroot),qPrintable(encoding),maxAge);
    maxCachedFileSize=settings->value("maxCachedFileSize","65536").toInt();
    cache.setMaxCost(settings->value("cacheSize","1000000").toInt());
    cacheTimeout=settings->value("cacheTime","60000").toInt();
    long int cacheMaxCost=(long int)cache.maxCost();
    qDebug("DownloadFileController: cache timeout=%i, size=%li",cacheTimeout,cacheMaxCost);
}


void DownloadFileController::service(HttpRequest &request, HttpResponse &response)
{
    QByteArray path=request.getPath();
    path.replace("/download","");
    // Check if we have the file in cache
    qint64 now=QDateTime::currentMSecsSinceEpoch();
    mutex.lock();
    CacheEntry* entry=cache.object(path);
    if (entry && (cacheTimeout==0 || entry->created>now-cacheTimeout))
    {
        QByteArray document=entry->document;
        QByteArray filename=entry->filename;
        mutex.unlock();
        qDebug("DownloadFileController: Cache hit for %s",path.data());
        setContentType(filename,response);
        response.setHeader("Cache-Control","max-age="+QByteArray::number(maxAge/1000));
        response.write(document);
    }
    else
    {
        mutex.unlock();
        // The file is not in cache.
        qDebug("DownloadFileController: Cache miss for %s",path.data());
        // Forbid access to files outside the docroot directory
        if (path.contains("/.."))
        {
            qWarning("DownloadFileController: detected forbidden characters in path %s",path.data());
            response.setStatus(403,"forbidden");
            response.write("403 forbidden",true);
            return;
        }
        // If the filename is a directory, append index.html.
        if (QFileInfo(docroot+path).isDir())
        {
            path+="/index.html";
        }
        // Try to open the file
        QFile file(docroot+path);
        qDebug("DownloadFileController: Open file %s",qPrintable(file.fileName()));
        if (file.open(QIODevice::ReadOnly))
        {
            setContentType(path,response);
            response.setHeader("Cache-Control","max-age="+QByteArray::number(maxAge/1000));
            if (file.size()<=maxCachedFileSize)
            {
                // Return the file content and store it also in the cache
                entry=new CacheEntry();
                while (!file.atEnd() && !file.error())
                {
                    QByteArray buffer=file.read(65536);
                    response.write(buffer);
                    entry->document.append(buffer);
                }
                entry->created=now;
                entry->filename=path;
                mutex.lock();
                cache.insert(request.getPath(),entry,entry->document.size());
                mutex.unlock();
            }
            else
            {
                // Return the file content, do not store in cache
                while (!file.atEnd() && !file.error())
                {
                    response.write(file.read(65536));
                }
            }
            file.close();
        }
        else {
            if (file.exists())
            {
                qWarning("DownloadFileController: cannot open existing file %s for reading",qPrintable(file.fileName()));
                response.setStatus(403,"forbidden");
                response.write("403 forbidden",true);
            }
            else
            {
                response.setStatus(404,"not found");
                response.write("404 not found",true);
            }
        }
    }
}

void DownloadFileController::setContentType(const QString fileName, HttpResponse &response) const
{
    QFileInfo fileInfo(fileName);
    QString name=fileInfo.fileName();
    response.setHeader("Content-Disposition",QString("attachment;filename=%1").arg(name).toUtf8());
    if (fileName.endsWith(".png"))
    {
        response.setHeader("Content-Type", "image/png");
    }
    else if (fileName.endsWith(".jpg"))
    {
        response.setHeader("Content-Type", "image/jpeg");
    }
    else if (fileName.endsWith(".gif"))
    {
        response.setHeader("Content-Type", "image/gif");
    }
    else if (fileName.endsWith(".pdf"))
    {
        response.setHeader("Content-Type", "application/pdf");
    }
    else if (fileName.endsWith(".txt"))
    {
        response.setHeader("Content-Type", qPrintable("text/plain; charset="+encoding));
    }
    else if (fileName.endsWith(".html") || fileName.endsWith(".htm"))
    {
        response.setHeader("Content-Type", qPrintable("text/html; charset="+encoding));
    }
    else if (fileName.endsWith(".css"))
    {
        response.setHeader("Content-Type", "text/css");
    }
    else if (fileName.endsWith(".js"))
    {
        response.setHeader("Content-Type", "text/javascript");
    }
    else if (fileName.endsWith(".svg"))
    {
        response.setHeader("Content-Type", "image/svg+xml");
    }
    else if (fileName.endsWith(".woff"))
    {
        response.setHeader("Content-Type", "font/woff");
    }
    else if (fileName.endsWith(".woff2"))
    {
        response.setHeader("Content-Type", "font/woff2");
    }
    else if (fileName.endsWith(".ttf"))
    {
        response.setHeader("Content-Type", "application/x-font-ttf");
    }
    else if (fileName.endsWith(".eot"))
    {
        response.setHeader("Content-Type", "application/vnd.ms-fontobject");
    }
    else if (fileName.endsWith(".otf"))
    {
        response.setHeader("Content-Type", "application/font-otf");
    }
    else if (fileName.endsWith(".json"))
    {
        response.setHeader("Content-Type", "application/json");
    }
    else if (fileName.endsWith(".xml"))
    {
        response.setHeader("Content-Type", "text/xml");
    }
    // Todo: add all of your content types
    else
    {
        qDebug("DownloadFileController: unknown MIME type for filename '%s'", qPrintable(fileName));
    }
}

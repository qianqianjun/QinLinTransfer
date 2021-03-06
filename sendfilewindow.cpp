#include "sendfilewindow.h"
#include "ui_sendfilewindow.h"

SendFileWindow::SendFileWindow(QVector<DeviceInfo> infos,QWidget *parent):
    QDialog(parent),ui(new Ui::sendFileWindow),targetDevices(infos),hintBox(this){

    this->setWindowModality(Qt::ApplicationModal);
    ui->setupUi(this);
    setAcceptDrops(true);
    initTargetDevice();
    this->manager=new SendFileManager(targetDevices,this);
    initSelectedFileArea();
    connect(ui->send_btn,&QPushButton::clicked,this,&SendFileWindow::sendFile);
    connect(&socketTimeoutTimer, &QTimer::timeout, this, &SendFileWindow::socketTimeout);
    socketTimeoutTimer.setSingleShot(true);
}

void SendFileWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void SendFileWindow::dropEvent(QDropEvent *event)
{
    if(event->mimeData()->hasUrls()){
        foreach(const QUrl &url,event->mimeData()->urls()){
            QString path=url.toLocalFile();
            if(path.endsWith("/")){
                path.remove(path.size()-1,1);
            }
            QFileInfo info(path);
            if(info.isFile()){
                this->manager->addFile(info.filePath());
            }
            else{
                QQueue<QFileInfo> dirInfos;
                dirInfos.append(info);
                while(!dirInfos.empty()){
                    QFileInfo temp=dirInfos.takeFirst();
                    QDir dir(temp.filePath());
                    QFileInfoList fileInfos=dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
                    for(QFileInfo& elem:fileInfos){
                        if(elem.isFile()){
                            this->manager->addFile(elem.filePath(),info.absolutePath());
                        }else{
                            dirInfos.append(elem);
                        }
                    }
                }
            }
            renderselectedFiles();
        }
    }
}

void SendFileWindow::initTargetDevice(){
    ui->device_table->setSelectionBehavior ( QAbstractItemView::SelectRows); //????????????????????????????????????
    ui->device_table->setSelectionMode ( QAbstractItemView::SingleSelection); //?????????????????????????????????
    ui->device_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->device_table->setColumnCount(2);
    ui->device_table->setHorizontalHeaderLabels(QStringList()<<"????????????"<<"IP??????");
    // ui->device_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->device_table->horizontalHeader()->setStretchLastSection(true);
    ui->device_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    ui->device_table->setRowCount(targetDevices.size());
    for(int i=0;i<targetDevices.size();i++){
        int col=0;
        ui->device_table->setItem(i,col++,new QTableWidgetItem(targetDevices[i].name));
        ui->device_table->setItem(i,col++,new QTableWidgetItem(targetDevices[i].ip));
    }
}


void SendFileWindow::initSelectedFileArea(){
    ui->files_table->setSelectionBehavior ( QAbstractItemView::SelectRows); //????????????????????????????????????
    ui->files_table->setSelectionMode ( QAbstractItemView::SingleSelection); //?????????????????????????????????
    ui->files_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->files_table->setColumnCount(2);
    ui->files_table->setHorizontalHeaderLabels(QStringList()<<"????????????"<<"????????????");
    ui->files_table->horizontalHeader()->setStretchLastSection(true);
    ui->files_table->horizontalHeader()->setMinimumWidth(185);
    ui->files_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    connect(ui->files_table,&QTableWidget::cellClicked,this->manager,&SendFileManager::changeIndex);
    connect(ui->remove_btn,&QPushButton::clicked,[=](){
        if(this->manager->selectedIndex==-1){
            QMessageBox::critical(nullptr, QApplication::applicationName(),
                                 QString("??????????????????????????????"));
        }else{
            ui->files_table->removeRow(this->manager->selectedIndex);
            ui->files_table->setCurrentItem(NULL);
            this->manager->removeFile();
            renderselectedFiles();
        }
    });
    connect(ui->select_file_btn,&QToolButton::clicked,[=](){
        QString path=QFileDialog::getExistingDirectory(this,"???????????????????????????");
        if(!path.isEmpty()){
            QFileInfo root(path);
            QQueue<QFileInfo> dirInfos;
            dirInfos.append(root);
            while(!dirInfos.empty()){
                QFileInfo temp=dirInfos.takeFirst();
                QDir dir(temp.filePath());
                QFileInfoList fileInfos=dir.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot);
                for(QFileInfo& elem:fileInfos){
                    if(elem.isFile()){
                        this->manager->addFile(elem.filePath(),root.absolutePath());
                    }else{
                        dirInfos.append(elem);
                    }
                }
            }
            renderselectedFiles();
        }
    });

    connect(ui->manage_btn,&QPushButton::clicked,this,&SendFileWindow::openManageWindow);
}

void SendFileWindow::openManageWindow(){
    ManageWindow* window=new ManageWindow(this->manager->fileInfos,this);
    window->show();
    connect(window,&ManageWindow::saveSetting,this,&SendFileWindow::updateList);
}

void SendFileWindow::updateList(QList<int> deleteIndexs)
{

    std::sort(deleteIndexs.begin(),deleteIndexs.end());
    for(int i=0;i<deleteIndexs.size();i++){
        deleteIndexs[i]-=i;
    }
    for(int i=0;i<deleteIndexs.size();i++){
        this->manager->fileInfos.removeAt(deleteIndexs[i]);
        this->manager->files.removeAt(deleteIndexs[i]);
    }
    renderselectedFiles();
}

void SendFileWindow::renderselectedFiles(){
    ui->files_table->clearContents();
    ui->files_table->setRowCount(this->manager->fileInfos.size());
    for(int i=0;i<this->manager->fileInfos.size();i++){
        int col=0;
        ui->files_table->setItem(i,col++,new QTableWidgetItem(this->manager->fileInfos[i].fileName));
        ui->files_table->setItem(i,col++,new QTableWidgetItem(this->manager->fileInfos[i].fileSize));
    }
}

SendFileWindow::~SendFileWindow(){
    delete ui;
}

void SendFileWindow::socketTimeout(){
    socket->disconnectFromHost();
    socket->close();
    socket->deleteLater();
    QMessageBox::critical(this, QApplication::applicationName(), "???????????????");
    ui->send_btn->setEnabled(true);
    setCursor(QCursor(Qt::ArrowCursor));
    sendFile();
}

void SendFileWindow::socketConnected(){
    socketTimeoutTimer.stop();
    SenderContext *sender=new SenderContext(socket,this->manager->files,this->manager->fileInfos,nullptr);
    ProgressBarUI *progressBar=new ProgressBarUI(sender,nullptr);
    connect(progressBar,&ProgressBarUI::openNextTask,this,&SendFileWindow::sendFile);
    connect(progressBar,&ProgressBarUI::transferInterruptedByUser,this,&SendFileWindow::close);
    progressBar->setAttribute(Qt::WA_DeleteOnClose);
    progressBar->show();
}

void SendFileWindow::sendFile(){
    // ?????????????????????????????????
    if(this->manager->files.size()<=0){
        QMessageBox::critical(nullptr,"????????????","?????????????????????????????????");
    }else{
        if(!targetDevices.empty()){
            DeviceInfo& info=targetDevices.front();
            socket = new QTcpSocket(this);
            connect(socket, &QTcpSocket::connected, this, &SendFileWindow::socketConnected);
            socket->connectToHost(info.addr, info.port);
            ui->send_btn->setEnabled(false);
            setCursor(QCursor(Qt::WaitCursor));
            socketTimeoutTimer.start(5000);
            targetDevices.pop_front();
        }else{
            hintBox.setIcon(QMessageBox::Information);
            hintBox.setWindowTitle(QApplication::applicationName());
            hintBox.setText("???????????????");
            hintBox.show();
            connect(&hintBox,&QMessageBox::finished,this,&ProgressBarUI::done);
            QTimer::singleShot(5000,&hintBox,&QMessageBox::accept);
        }
    }
}

//===================================================================================================
SendFileManager::SendFileManager(QVector<DeviceInfo> targetDevices,
                                 QObject* parent):
    QObject(parent),targetDevices(targetDevices),selectedIndex(-1){
}
void SendFileManager::changeIndex(int row,int col){
    col++;
    this->selectedIndex=row;
}
QString parseSize(qint64 size){
    QStringList units;
    units<<"B"<<"KB"<<"MB"<<"GB"<<"TB";
    int i=0;
    double capacity=size;
    while(capacity>1024){
        i++;
        capacity/=1024;
    }
    if(static_cast<int>(capacity)==capacity){
        return QString("%1%2").arg(QString::number(static_cast<int>(capacity))).arg(units[i]);
    }
    return QString("%1%2").arg(QString::number(capacity,'f',2)).arg(units[i]);
}
void SendFileManager::addFile(const QString &filepath,QString root){
    // ?????????????????????????????????
    foreach (QSharedPointer<QFile> file, files) {
        if (file->fileName() == filepath)
            return;
    }
    QSharedPointer<QFile> fp = QSharedPointer<QFile>::create(filepath);
    if (!fp->open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, QApplication::applicationName(),"???????????????????????????");
        return;
    }
    if (fp->isSequential()) {
        QMessageBox::critical(nullptr, QApplication::applicationName(),"??????????????????????????????");
        return;
    }
    files.append(fp);
    QFileInfo info(filepath);
    QString filename;
    if(root==nullptr){
        filename=info.fileName();
    }else{
        filename=info.absoluteFilePath().replace(root+"/","");
    }
    fileInfos.append(FileInfo(filename,parseSize(info.size())));
}
void SendFileManager::removeFile(){
    if(selectedIndex>=0 && selectedIndex<files.size()){
        this->fileInfos.removeAt(selectedIndex);
        this->files.removeAt(selectedIndex);
        selectedIndex=-1;
    }
}

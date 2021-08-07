#include "sendfilewindow.h"
#include "ui_sendfilewindow.h"
#include <QDebug>
SendFileWindow::SendFileWindow(QVector<QString> names,QVector<QString> ips
                               ,QWidget *parent):
    QDialog(parent),names(names),ips(ips),ui(new Ui::sendFileWindow){
    ui->setupUi(this);
    initTargetDevice();
    this->manager=new SendFileManager(this);
    initSelectedFileArea();
}

void SendFileWindow::initTargetDevice(){
    connect(ui->cancle_btn,&QPushButton::clicked,this,&QWidget::close);
    ui->device_table->setSelectionBehavior ( QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    ui->device_table->setSelectionMode ( QAbstractItemView::SingleSelection); //设置选择模式，选择单行
    ui->device_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->device_table->setColumnCount(2);
    ui->device_table->setHorizontalHeaderLabels(QStringList()<<"设备名称"<<"IP地址");
    ui->device_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->device_table->setRowCount(names.size());
    for(int i=0;i<names.size();i++){
        int col=0;
        ui->device_table->setItem(i,col++,new QTableWidgetItem(names[i]));
        ui->device_table->setItem(i,col++,new QTableWidgetItem(ips[i]));
    }
}

void SendFileWindow::initSelectedFileArea(){
    ui->files_table->setSelectionBehavior ( QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    ui->files_table->setSelectionMode ( QAbstractItemView::SingleSelection); //设置选择模式，选择单行
    ui->files_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->files_table->setColumnCount(2);
    ui->files_table->setHorizontalHeaderLabels(QStringList()<<"文件名称"<<"文件大小");
    ui->files_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(ui->files_table,&QTableWidget::cellClicked,this->manager,&SendFileManager::changeIndex);
    connect(ui->remove_btn,&QPushButton::clicked,[=](){
        if(this->manager->selectedIndex==-1){
            QMessageBox::critical(nullptr, QApplication::applicationName(),
                                 QString("请先选中要删除的文件"));
        }else{
            ui->files_table->removeRow(this->manager->selectedIndex);
            ui->files_table->setCurrentItem(NULL);
            this->manager->removeFile();
            renderselectedFiles();
        }
    });
    connect(ui->select_file_btn,&QToolButton::clicked,[=](){

        QStringList filenames = QFileDialog::getOpenFileNames(this, QString("选择要发送的文件"));
        if(!filenames.empty()){
            foreach (const QString &filename, filenames) {
                this->manager->addFile(filename);
            }
            renderselectedFiles();
        }
    });
}

void SendFileWindow::renderselectedFiles(){
    // clear content of the table first
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

/**
 * @brief SendFileManager::SendFileManager 的相关实现函数
 * @param parent
 */
SendFileManager::SendFileManager(QObject* parent):QObject(parent),selectedIndex(-1){

}
void SendFileManager::changeIndex(int row,int col){
    this->selectedIndex=row;
}

void SendFileManager::addFile(const QString &filename){
    foreach (QSharedPointer<QFile> file, files) {
        if (file->fileName() == filename)
            return;
    }

    QSharedPointer<QFile> fp = QSharedPointer<QFile>::create(filename);
    if (!fp->open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, QApplication::applicationName(),"无法打开所选文件！");
        return;
    }
    if (fp->isSequential()) {
        QMessageBox::critical(nullptr, QApplication::applicationName(),"所选文件格式不支持！");
        return;
    }
    files.append(fp);
    QFile* file=fp.data();
    fileInfos.append(FileInfo(file->fileName().split("/").last(),parseSize(file->size())));
}
QString SendFileManager::parseSize(qint64 size){
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
void SendFileManager::removeFile(){
    if(selectedIndex>=0 && selectedIndex<files.size()){
        this->fileInfos.removeAt(selectedIndex);
        this->files.removeAt(selectedIndex);
        selectedIndex=-1;
    }
}

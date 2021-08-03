#include "sendfilewindow.h"
#include "ui_sendfilewindow.h"
SendFileWindow::SendFileWindow(QVector<QString> names,QVector<QString> ips
                               ,QWidget *parent):
    QDialog(parent),names(names),ips(ips),ui(new Ui::sendFileWindow){
    ui->setupUi(this);
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

    this->manager=new SendFileManager(this);


    ui->files_table->setSelectionBehavior ( QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    ui->files_table->setSelectionMode ( QAbstractItemView::SingleSelection); //设置选择模式，选择单行
    ui->files_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->files_table->setColumnCount(2);
    ui->files_table->setHorizontalHeaderLabels(QStringList()<<"文件名称"<<"文件大小");
    ui->files_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->files_table->setRowCount(this->manager->files.size());
    for(int i=0;i<this->manager->files.size();i++){
        int col=0;
        ui->files_table->setItem(i,col++,new QTableWidgetItem(this->manager->files[i].fileName));
        ui->files_table->setItem(i,col++,new QTableWidgetItem(this->manager->files[i].fileSize));
    }

    connect(ui->remove_btn,&QPushButton::clicked,[=](){
        if(this->manager->selectedIndex==-1){
            QMessageBox::critical(nullptr, QApplication::applicationName(),
                                 QString("请先选中要删除的文件"));
        }else{
            ui->files_table->removeRow(this->manager->selectedIndex);
            this->manager->files.remove(this->manager->selectedIndex);
            this->manager->selectedIndex=-1;
        }
    });

    connect(ui->files_table,&QTableWidget::cellClicked,this->manager,&SendFileManager::changeIndex);
}

SendFileWindow::~SendFileWindow(){
    delete ui;
}

SendFileManager::SendFileManager(QObject* parent):QObject(parent),selectedIndex(-1){
    this->files.push_back(FileInfo("我的很大.doc","1024T"));
    this->files.push_back(FileInfo("你忍一下.doc","2048"));
    this->files.push_back(FileInfo("我的很大.ppt","512G"));
    this->files.push_back(FileInfo("你忍一下.pptx","256G"));
}
void SendFileManager::changeIndex(int row,int col){
    this->selectedIndex=row;
}

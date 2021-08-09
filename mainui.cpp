/*
 * @author qianqianjun
 * @date 2021.07
*/
#include "mainui.h"
#include "ui_mainui.h"
#include "navigater.h"
#include "websend.h"
#include "webreceive.h"

QHostAddress getLocalHostIP()
{
  QList<QHostAddress> AddressList = QNetworkInterface::allAddresses();
  QHostAddress result;
  foreach(QHostAddress address, AddressList){
      if(address.protocol() == QAbstractSocket::IPv4Protocol &&
         address != QHostAddress::Null &&
         address != QHostAddress::LocalHost){
          if (address.toString().contains("127.0.")){
            continue;
          }
          result = address;
          break;
      }
  }
  return result;
}

MainUI::MainUI(DiscoveryService* discoverService,QWidget *parent) : QMainWindow(parent),
    ui(new Ui::MainUI),discoverService(discoverService){
    ui->setupUi(this);
    initUserInfo();
    initialLeftMenu();
    initWebPage();
    manager=new DeviceManager(ui->multi_transfer_btn,ui->select_all,ui->devices_list_widget);
    manager->startAsking(discoverService);
    manager->renderOnlinePage();
    connect(this->discoverService,&DiscoveryService::newHost,this->manager,&DeviceManager::updateDeviceList);
    initSettingPage();
}

// 构造函数需要的函数
void MainUI::initUserInfo(){
    ui->nickname_label->setText(Settings::deviceName());
    ui->port_label->setText(QString::number(Settings::serverPort()));
    ui->ip_label->setText(getLocalHostIP().toString());
}
void MainUI::initialLeftMenu(){
    QVBoxLayout *layout=new QVBoxLayout();
    NavigaterLabel * online=new NavigaterLabel(0,"在线设备");
    NavigaterLabel * web=new NavigaterLabel(1,"WEB传输");
    NavigaterLabel *setting=new NavigaterLabel(2,"系统设置");
    QVector<NavigaterLabel*> labels;
    labels.push_back(online);labels.push_back(web);labels.push_back(setting);
    this->navigater=new Navigater(ui->stackedWidget,labels,this);
    QFrame *line = new QFrame();
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Plain);
    line->setLineWidth(2);
    layout->addWidget(line);
    layout->addWidget(online);
    layout->addWidget(web);
    layout->addWidget(setting);
    ui->menu_widget->setLayout(layout);
}
void MainUI::initSettingPage(){
    ui->set_devicename->setText(Settings::deviceName());
    ui->set_download_path->setText(Settings::downloadPath());
    ui->set_discovery_port->setText(QString::number(Settings::DiscoveryPort()));
    ui->set_transfer_port->setText(QString::number(Settings::serverPort()));
    ui->set_visable_checkbox->setChecked(Settings::discoverable());

    connect(ui->set_save_btn,&QPushButton::clicked,this,&MainUI::settingSave);
    connect(ui->set_cancle_btn,&QPushButton::clicked,this,&MainUI::settingCancle);
    connect(ui->set_choosedir_btn,&QPushButton::clicked,this,&MainUI::chooseDownloadDir);
}
void MainUI::initWebPage()
{
    connect(ui->sendbtn,&QPushButton::clicked,this,&MainUI::openTransferWindow);
    connect(ui->sendbtn_2,&QPushButton::clicked,this,&MainUI::openReceiverWindow);
    connect(ui->send,&QGroupBox::clicked,this,&MainUI::openTransferWindow);
}
// 设置界面需要的函数
void MainUI::settingSave(){
    QStringList wrongItems;
    if(ui->set_devicename->text()=="") wrongItems.push_back("设备名称");
    QString tport=ui->set_transfer_port->text();
    if(tport=="" || tport.toInt()<0 || tport.toInt()>65535) wrongItems.push_back("传输端口");
    QString dport=ui->set_discovery_port->text();
    if(dport=="" || dport.toInt()<0 || dport.toInt()>65535) wrongItems.push_back("发现端口");
    if(ui->set_download_path->text()=="") wrongItems.push_back("下载路径");

    if(wrongItems.size()>0){
        QString msg;
        for(int i=0;i<wrongItems.size()-1;i++){
            msg+=wrongItems[i]+"，";
        }
        msg+=wrongItems[wrongItems.size()-1];
        QMessageBox::critical(nullptr,"提示信息",QString("%1的设置格式有误！\n（所有项目不能为空，端口号范围为0~65535）").arg(msg));
        return;
    }


    QStringList changeItems;
    if(ui->set_devicename->text()!=Settings::deviceName()){
        Settings::setDeviceName(ui->set_devicename->text());
        ui->nickname_label->setText(Settings::deviceName());
    }
    if(ui->set_transfer_port->text().toUInt()!=Settings::serverPort()){
        Settings::setServerPort(ui->set_transfer_port->text().toUInt());
        changeItems<<"传输端口";
    }
    if(ui->set_discovery_port->text().toUInt()!=Settings::DiscoveryPort()){

        Settings::setDiscoveryPort(ui->set_discovery_port->text().toUInt());
        changeItems<<"发现端口";
    }
    if(ui->set_visable_checkbox->isChecked()!=Settings::discoverable()){
        Settings::setDiscoverable(ui->set_visable_checkbox->isChecked());
    }
    if(ui->set_download_path->text()!=Settings::downloadPath()){
        Settings::setDownloadPath(ui->set_download_path->text());
    }
    if(changeItems.size()>0){
        QString s;
        if(changeItems.size()==1)
            s=changeItems[0];
        else{
            s=changeItems[0]+"，"+changeItems[1];
        }
        QMessageBox::information(nullptr,"提示信息",QString("设置保存成功！\n%1的设置将在下次重启生效！").arg(s));
    }else{
        QMessageBox::information(nullptr,"提示信息","设置保存成功");
    }
}
void MainUI::settingCancle(){
    ui->set_devicename->setText(Settings::deviceName());
    ui->set_download_path->setText(Settings::downloadPath());
    ui->set_discovery_port->setText(QString::number(Settings::DiscoveryPort()));
    ui->set_transfer_port->setText(QString::number(Settings::serverPort()));
    ui->set_visable_checkbox->setChecked(Settings::discoverable());
    QMessageBox::information(nullptr,"提示信息","设置已还原！");
}
void MainUI::chooseDownloadDir(){
    QString path=ui->set_download_path->text();
    if(path==""){
        path=QStandardPaths::DownloadLocation;
    }
    QString dir=QFileDialog::getExistingDirectory(this,"",path);
    if(!dir.isEmpty()){
        ui->set_download_path->setText(dir);
    }
}
void MainUI::closeEvent(QCloseEvent *event){
    event->type();
    emit closeWindow();
}
MainUI::~MainUI(){
    delete ui;
    delete navigater;
    delete manager;
}

// web传输界面需要的函数
void MainUI::openTransferWindow(){
    websend *webs=new websend();
    webs->show();
}
void MainUI::openReceiverWindow()
{
    webreceive *webr=new webreceive();
    webr->show();
}

// 外部调用的函数
void MainUI::setPageIndex(int index){
    if(ui->stackedWidget->count()>index){
        this->navigater->selected(index);
    }else{
        qDebug()<<"页数超过范围！";
    }
}

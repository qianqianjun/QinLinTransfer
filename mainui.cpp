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
}
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
void MainUI::closeEvent(QCloseEvent *event){
    event->type();
    emit closeWindow();
}
MainUI::~MainUI(){
    delete ui;
    delete navigater;
    delete manager;
}

void MainUI::openTransferWindow(){
    websend *webs=new websend();
    webs->show();
}

void MainUI::openReceiverWindow()
{
    webreceive *webr=new webreceive();
    webr->show();
}


void MainUI::initWebPage()
{
    connect(ui->sendbtn,&QPushButton::clicked,this,&MainUI::openTransferWindow);
    connect(ui->sendbtn_2,&QPushButton::clicked,this,&MainUI::openReceiverWindow);
    connect(ui->send,&QGroupBox::clicked,this,&MainUI::openTransferWindow);
}


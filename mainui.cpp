/*
 * @author qianqianjun
 * @date 2021.07
*/
#include "mainui.h"
#include "ui_mainui.h"
#include "navigater.h"
#include "websend.h"
#include "webreceive.h"
MainUI::MainUI(QWidget *parent) : QMainWindow(parent),ui(new Ui::MainUI){
    ui->setupUi(this);
    initialLeftMenu();

    QVector<DeviceInfo> devices;
    devices.push_back(DeviceInfo("荣耀手机","192.168.31.34","53645"));
    devices.push_back(DeviceInfo("Surface","192.168.31.226","8080"));
    initialOnlinePage(devices);
    initWebPage();
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
void MainUI::initialOnlinePage(const QVector<DeviceInfo>& devices){
    QVBoxLayout* layout=new QVBoxLayout(this);
    manager=new DeviceManager(ui->multi_transfer_btn,ui->select_all);
    QVector<QString> heads;
    heads<<"选择"<<"设备名称"<<"Ip地址"<<"端口号"<<"操作";
    Title* title=new Title(heads,this);
    layout->addWidget(title);

    for(int i=0;i<devices.size();i++){
        OnlineDeviceItem* item=new OnlineDeviceItem(devices[i],ui->devices_list_widget);
        manager->addOnlineDeviceItem(item);
        layout->addWidget(item);
    }
    manager->addConnectionAction();
    layout->addStretch();
    ui->devices_list_widget->setLayout(layout);
}
MainUI::~MainUI(){
    delete ui;
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


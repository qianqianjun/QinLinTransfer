/*
 * @author qianqianjun
 * @date 2021.07
*/
#include "onlinedeviceitem.h"
#include <QDebug>
// OnlineDeviceItem 相关的函数实现
OnlineDeviceItem::OnlineDeviceItem(DeviceInfo info,QWidget *parent) : QWidget(parent),info(info)
{
    QHBoxLayout* layout=new QHBoxLayout(parent);
    this->deviceName=new QLabel(info.name,this);
    this->ip=new QLabel(info.ip,this);
    this->port=new QLabel(info.port,this);
    this->btn=new QPushButton("传文件",this);
    this->checkbox=new QCheckBox(this);
    layout->addWidget(this->checkbox);
    layout->addWidget(this->deviceName);
    layout->addWidget(this->ip);
    layout->addWidget(this->port);
    layout->addWidget(this->btn);
    this->setLayout(layout);
}
DeviceInfo OnlineDeviceItem::getDeviceInfo(){
    return this->info;
}

//  Title 相关的函数实现

Title::Title(QVector<QString> heads,QWidget* parent):QWidget(parent),titles(heads){
    for(int i=0;i<heads.size();i++){
        this->labels.push_back(new QLabel(heads[i],this));
    }
    QHBoxLayout* layout=new QHBoxLayout(parent);
    for(int i=0;i<labels.size();i++) layout->addWidget(labels[i]);
    this->setLayout(layout);
}

// DeviceManager 相关的函数实现
DeviceManager::DeviceManager(QToolButton* multi_transfer_btn,
                             QToolButton* select_all_btn,
                             QObject* parent):QObject(parent),
    multiTransferBtn(multi_transfer_btn),selectAllBtn(select_all_btn){
    connect(this->selectAllBtn,&QToolButton::clicked,this,&DeviceManager::selectAllDevices);
    connect(this->multiTransferBtn,&QToolButton::clicked,[=](){
        openTransferWindow(this->selectedDevices);
    });
}

void DeviceManager::addOnlineDeviceItem(OnlineDeviceItem*& item){
    this->onlineDeviceItems.push_back(item);
}

void DeviceManager::addConnectionAction(){
    for(int i=0;i<this->onlineDeviceItems.size();i++){
        connect(this->onlineDeviceItems[i]->checkbox,&QCheckBox::stateChanged,[=](int state){
            if(state==0){
                removeTransferDevice(this->onlineDeviceItems[i]->getDeviceInfo());
            }else{
                addTransferDevice(this->onlineDeviceItems[i]->getDeviceInfo());
            }
//            qDebug()<<"the value in the selected devices";
//            for(int i=0;i<this->selectedDevices.size();i++){
//                qDebug()<<this->selectedDevices[i].name;
//            }
        });

        connect(this->onlineDeviceItems[i]->btn,&QPushButton::clicked,[=](){
            QVector<DeviceInfo> devices;
            devices.push_back(this->onlineDeviceItems[i]->getDeviceInfo());
            openTransferWindow(devices);
        });
    }
}

void DeviceManager::addTransferDevice(const DeviceInfo& info){
    this->selectedDevices.push_back(info);
}
void DeviceManager::removeTransferDevice(const DeviceInfo& info){
    for(int i=0;i<this->selectedDevices.size();i++){
        if(this->selectedDevices[i].ip==info.ip && this->selectedDevices[i].port==info.port){
            this->selectedDevices.remove(i);
            return;
        }
    }
}

QVector<DeviceInfo> DeviceManager::getSelectedDevices(){
    return this->selectedDevices;
}

void DeviceManager::selectAllDevices(){
    for(int i=0;i<this->onlineDeviceItems.size();i++){
        if(this->onlineDeviceItems[i]->checkbox->checkState()!=Qt::Checked){
            this->onlineDeviceItems[i]->checkbox->setCheckState(Qt::Checked);
        }
    }
}

void DeviceManager::openTransferWindow(QVector<DeviceInfo> devices){
    QVector<QString> names;
    QVector<QString> ips;
    for(int i=0;i<devices.size();i++){
        names.push_back(devices[i].name);
        ips.push_back(devices[i].ip);
    }
    SendFileWindow* sendFileWindow=new SendFileWindow(names,ips,nullptr);
    sendFileWindow->show();
}

















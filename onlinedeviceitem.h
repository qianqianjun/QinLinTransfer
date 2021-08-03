#ifndef ONLINEDEVICEITEM_H
#define ONLINEDEVICEITEM_H
/*
 * @author qianqianjun
 * @date 2021.07
*/
#include <QObject>
#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QString>
#include <QCheckBox>
#include <QHBoxLayout>
#include <QToolButton>
#include <sendfilewindow.h>
typedef struct data{
    QString name;
    QString ip;
    QString port;
    data(QString name,QString ip,QString port):name(name),ip(ip),port(port){}
}DeviceInfo;

class OnlineDeviceItem : public QWidget
{
    Q_OBJECT
private:
    QLabel* deviceName;
    QLabel* ip;
    QLabel* port;
    DeviceInfo info;
public:
    QCheckBox * checkbox;
    QPushButton* btn;
    explicit OnlineDeviceItem(DeviceInfo info,QWidget *parent = nullptr);
    DeviceInfo getDeviceInfo();
};


class Title:public QWidget{
    Q_OBJECT
private:
    QVector<QString> titles;
    QVector<QLabel*> labels;
public:
    explicit Title(QVector<QString> heads,QWidget* parent=nullptr);
};

class DeviceManager:public QObject{
    Q_OBJECT
private:
    QVector<DeviceInfo> selectedDevices;
    QVector<OnlineDeviceItem*> onlineDeviceItems;
    QToolButton* multiTransferBtn;
    QToolButton* selectAllBtn;
public:
    DeviceManager(QToolButton* mult_transfer_btn,QToolButton* select_all_btn,QObject* parent=nullptr);
    QVector<DeviceInfo> getSelectedDevices();
    void addOnlineDeviceItem(OnlineDeviceItem*& item);
    void addConnectionAction();
public slots:
    void addTransferDevice(const DeviceInfo& info);
    void removeTransferDevice(const DeviceInfo& info);
    void selectAllDevices();
    void openTransferWindow(QVector<DeviceInfo> devices);
};

#endif // ONLINEDEVICEITEM_H

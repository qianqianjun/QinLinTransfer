#ifndef MAINUI_H
#define MAINUI_H

#include <QMainWindow>
#include <navigater.h>
#include <QLayout>
#include <QPushButton>
#include <onlinedeviceitem.h>
#include <QSpacerItem>
#include <QCloseEvent>
#include <QNetworkInterface>
#include <discoveryservice.h>
#include <settings.h>
namespace Ui {
class MainUI;
}

class MainUI : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainUI(DiscoveryService* discoverService,QWidget *parent = nullptr);
    ~MainUI();
signals:
    void closeWindow();
protected:
    void closeEvent(QCloseEvent *event);
private:
    Navigater* navigater;
    Ui::MainUI *ui;
    DeviceManager* manager;
    DiscoveryService* discoverService;
    void initialLeftMenu();
    void initUserInfo();
};

#endif // MAINUI_H

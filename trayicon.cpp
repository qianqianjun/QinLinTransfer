#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QIcon>
#include <QNetworkProxy>
#include <QTimer>
#include <QUrl>
#include "selectfilesdialog.h"
#include "settings.h"
#include "trayicon.h"

// 这里是系统托盘实现。
TrayIcon::TrayIcon(QObject *parent) : QSystemTrayIcon(parent)
{
    QNetworkProxy::setApplicationProxy(QNetworkProxy::NoProxy);
    QIcon appIcon(":/icons/app.png");
    QIcon appMaskIcon(":/icons/app_mask.png");
    // Indicate that this icon is a mask image(boolean isMask), and hence can potentially be modified based on where it's displayed.
    appMaskIcon.setIsMask(true);
    QIcon sendIcon(":/icons/send.png");
    QIcon openDownloadFolderIcon(":/icons/open_download_folder.png");
    QIcon settingsIcon(":/icons/settings.png");
    QIcon aboutIcon(":/icons/about.png");
    QIcon exitIcon(":/icons/exit.png");
    if (QSysInfo::productType() == "osx" || QSysInfo::productType() == "macos")
        setIcon(appMaskIcon);
    else
        setIcon(appIcon);
    //QAction 是菜单项目
    QAction *action, *addrPortAction;
    addrPortAction = menu.addAction("");
    addrPortAction->setEnabled(false);
    menu.addSeparator();
    action = menu.addAction(aboutIcon, "主界面");
    connect(action, &QAction::triggered, this, &TrayIcon::showMainWindow);
    action = menu.addAction(openDownloadFolderIcon, "下载目录");
    connect(action, &QAction::triggered, this, &TrayIcon::openDownloadFolderActionTriggered);
    action = menu.addAction(sendIcon, "发送文件");
    connect(action, &QAction::triggered, this, &TrayIcon::sendActionTriggered);
    action = menu.addAction(settingsIcon, "设置");
    connect(action, &QAction::triggered, &settingsDialog, &SettingsDialog::show);
    menu.addSeparator();

    action = menu.addAction(exitIcon, "退出");
    connect(action, &QAction::triggered, this, &TrayIcon::exitActionTriggered);
    setContextMenu(&menu);

    setToolTip(QApplication::applicationName());
    // QSystemTrayIcon::activated 当双击或者其他情况被激活。
    connect(this, &QSystemTrayIcon::activated, this, &TrayIcon::trayIconActivated);

    // 启动传送服务器，监听一个端口，这个端口默认是随机的。FileTransferServer类型，成员变量
    server.start(); // 检查端口是否被占用，设置当新的TCP连接建立之后，执行的操作。
    addrPortAction->setText("端口: " + QString::number(server.port()));

    this->haveUi=true;
    // 启动邻居发现服务
    discoveryService.start(server.port());
    DiscoveryService* dp=&discoveryService;
    mainui=new MainUI(dp);
    mainui->show();
    connect(mainui,&MainUI::closeWindow,this,&TrayIcon::mainuiClosed);
    // 提示信息，不重要
    QTimer::singleShot(0, this, [this]() {
        showMessage(QApplication::applicationName(), QApplication::applicationName() + " 在这里运行！");
    });
}

void TrayIcon::mainuiClosed(){
    this->haveUi=false;
    delete this->mainui;
}

void TrayIcon::sendActionTriggered()
{
    SelectFilesDialog *d = new SelectFilesDialog(nullptr, discoveryService);
    d->setAttribute(Qt::WA_DeleteOnClose);
    d->show();
}

void TrayIcon::openDownloadFolderActionTriggered()
{
    QString downloadPath = Settings::downloadPath();
    QDir().mkpath(downloadPath);
    QDesktopServices::openUrl(QUrl::fromLocalFile(downloadPath));
}

void TrayIcon::exitActionTriggered()
{
    QApplication::exit();
}


void TrayIcon::trayIconActivated(ActivationReason reason)
{
    if (reason == DoubleClick){
        this->showMainWindow();
    }
}

void TrayIcon::showMainWindow(){
    if(this->haveUi){
        mainui->activateWindow();
    }else{
        mainui=new MainUI(&discoveryService);
        mainui->show();
        connect(mainui,&MainUI::closeWindow,this,&TrayIcon::mainuiClosed);
        this->haveUi=true;
    }
}

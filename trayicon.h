#pragma once
#include <QMenu>
#include <QSystemTrayIcon>
#include "discoveryservice.h"
#include "filetransferserver.h"
#include "mainui.h"

class TrayIcon : public QSystemTrayIcon {
    Q_OBJECT
public:
    explicit TrayIcon(QObject *parent = nullptr);
public slots:
    void showOnlineDeviceWindow();
    void showSettingWindow();
    void onWindowClose();
private:
    QMenu menu;
    FileTransferServer server;
    DiscoveryService discoveryService;
    MainUI* mainui;
    bool haveUi;
private slots:
    void openDownloadFolder(); // 点击下载目录执行操作
    void exitApplication(); // 点击退出执行操作
    // 当被激活时，如果是被双击激活，则直接打开发送文件功能
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
};

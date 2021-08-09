#pragma once
#include <QMenu>
#include <QSystemTrayIcon>
#include "aboutdialog.h"
#include "discoveryservice.h"
#include "filetransferserver.h"
#include "settingsdialog.h"
#include "mainui.h"

class TrayIcon : public QSystemTrayIcon {
    Q_OBJECT
public:
    explicit TrayIcon(QObject *parent = nullptr);
public slots:
    void showMainWindow();
    void mainuiClosed();
private:
    QMenu menu;
    AboutDialog aboutDialog;
    SettingsDialog settingsDialog;
    FileTransferServer server;
    DiscoveryService discoveryService;
    MainUI* mainui;
    bool haveUi;
private slots:
    void sendActionTriggered(); // 点击发送文件的执行操作
    void openDownloadFolderActionTriggered(); // 点击下载目录执行操作
    void exitActionTriggered(); // 点击退出执行操作
    // 当被激活时，如果是被双击激活，则直接打开发送文件功能
    void trayIconActivated(QSystemTrayIcon::ActivationReason reason);
};

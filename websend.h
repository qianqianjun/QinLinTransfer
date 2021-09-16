#ifndef WEBSEND_H
#define WEBSEND_H

#include <QFileDialog>
#include <QString>
#include <QPainter>
#include <QBrush>
#include <QDebug>
#include <QLayout>
#include <QMessageBox>
#include <QWidget>
#include "qrencode/qrencode.h"
#include "qrcode.h"
#include <webserver.h>
#include <QFile>
#include <util.h>
#include <settings.h>

namespace Ui {
class websend;
}

class WebSend: public QWidget
{
    Q_OBJECT

public:
    explicit WebSend(WebServer*& webServer,QWidget *parent = nullptr);
    void removeOld();
    void updateUrl(const QString& str);
    ~WebSend();

private slots:
    void selectFileBtnClicked();
    void generateCodeBtnClicked();

private:
    Ui::websend *ui;
    QStringList filePaths;
    bool havaQrcode;
    QrcodeWidget* qrcodeWidget;
    WebServer* webServer;
};

#endif // WEBSEND_H

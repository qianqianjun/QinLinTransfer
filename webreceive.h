#ifndef WEBRECEIVE_H
#define WEBRECEIVE_H

#include <QWidget>
#include "qrencode/qrencode.h"
#include "qrcode.h"
#include <webserver.h>
#include <QString>
#include <QPainter>
#include <QBrush>
#include <QLayout>
#include <settings.h>
#include <QClipboard>
#include <QMessageBox>
namespace Ui {
class webreceive;
}

class WebReceive : public QWidget
{
    Q_OBJECT

public:
    explicit WebReceive(WebServer*& webServer,QWidget *parent = nullptr);
    void removeOld();
    ~WebReceive();
private slots:
    void generateCodeBtnClick();
    void copyBtnClick();

private:
    Ui::webreceive *ui;
    bool havaQrcode;
    QrcodeWidget* qrcodeWidget;
    WebServer* webServer;
};

#endif // WEBRECEIVE_H

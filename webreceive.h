#ifndef WEBRECEIVE_H
#define WEBRECEIVE_H

#include <QWidget>
#include "qrencode/qrencode.h"
#include "qrcode.h"

namespace Ui {
class webreceive;
}

class WebReceive : public QWidget
{
    Q_OBJECT

public:
    explicit WebReceive(QWidget *parent = nullptr);
    void removeOld();
    void updateUrl(const QString& str);
    ~WebReceive();
private slots:

    void on_pushButton_clicked();

private:
    Ui::webreceive *ui;
    QString url;
    bool havaQrcode;
    QrcodeWidget* qrcodeWidget;
};

#endif // WEBRECEIVE_H

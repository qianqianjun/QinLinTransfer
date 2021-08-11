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


namespace Ui {
class websend;
}

class WebSend : public QWidget
{
    Q_OBJECT

public:
    explicit WebSend(QWidget *parent = nullptr);
    void removeOld();
    void updateUrl(const QString& str);
    ~WebSend();

private slots:
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:

    Ui::websend *ui;
    QString url;
    bool havaQrcode;
    QrcodeWidget* qrcodeWidget;
};

#endif // WEBSEND_H

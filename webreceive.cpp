#include "webreceive.h"
#include "ui_webreceive.h"
#include <QString>
#include <QPainter>
#include <QBrush>
#include <QLayout>

WebReceive::WebReceive(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::webreceive),havaQrcode(false)
{
    ui->setupUi(this);
    setWindowTitle("接收文件");
    url=ui->comboBox->itemText(0);
    connect(ui->comboBox,&QComboBox::currentTextChanged,this,&WebReceive::updateUrl);
}

void WebReceive::removeOld()
{
    if(havaQrcode){
        delete ui->qrcode_area->layout();
        delete qrcodeWidget;
    }
}

void WebReceive::updateUrl(const QString &str)
{
    url=str;
}


WebReceive::~WebReceive()
{
    delete ui;
}


void WebReceive::on_pushButton_clicked()
{
    removeOld();
    havaQrcode=true;
    QVBoxLayout *layout=new QVBoxLayout(this);
    qrcodeWidget=new QrcodeWidget(this);
    qrcodeWidget->setUrl(url);
    layout->addWidget(qrcodeWidget);
    ui->qrcode_area->setLayout(layout);
    update();
}


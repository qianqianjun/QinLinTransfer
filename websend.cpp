#include "websend.h"
#include "ui_websend.h"
#include <QFile>
#include <websend.h>

WebSend::WebSend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::websend),havaQrcode(false)
{
    ui->setupUi(this);
    setWindowTitle("发送文件");
    connect(ui->lineEdit,&QLineEdit::textEdited,this,&WebSend::updateUrl);
}

void WebSend::removeOld()
{
    if(havaQrcode){
        delete ui->qrcode_area->layout();
        delete qrcodeWidget;
    }
}

void WebSend::updateUrl(const QString& str)
{
    url=str;
}

WebSend::~WebSend()
{
    delete ui;
}

void WebSend::on_pushButton_clicked()
{
    QString path=QFileDialog::getOpenFileName(this,"打开文件","./");
    ui->lineEdit->setText(path);
    url=path;
}


void WebSend::on_pushButton_2_clicked()
{
    if(url.isEmpty()){
        QMessageBox::critical(this,"错误","请选择文件");
    }
    removeOld();
    havaQrcode=true;
    QVBoxLayout *layout=new QVBoxLayout(this);
    qrcodeWidget=new QrcodeWidget(this);
    qrcodeWidget->setUrl(url);
    layout->addWidget(qrcodeWidget);
    ui->qrcode_area->setLayout(layout);
    update();
}


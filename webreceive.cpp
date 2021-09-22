#include "webreceive.h"
#include "ui_webreceive.h"

WebReceive::WebReceive(WebServer*& webServer,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::webreceive),havaQrcode(false),webServer(webServer)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowTitle("接收文件");
    QStringList ips=getLocalHostIP();
    for(int i=0;i<ips.size();i++){
        ui->ips_combox->addItem(ips[i]);
    }
    connect(ui->generateCodeBtn,&QPushButton::clicked,this,&WebReceive::generateCodeBtnClick);
    connect(ui->copyButton,&QPushButton::clicked,this,&WebReceive::copyBtnClick);
}

void WebReceive::removeOld(){
    if(havaQrcode){
        delete ui->qrcode_area->layout();
        delete qrcodeWidget;
    }
}

WebReceive::~WebReceive()
{
    delete ui;
}


void WebReceive::generateCodeBtnClick()
{
    removeOld();
    havaQrcode=true;
    QVBoxLayout *layout=new QVBoxLayout(this);
    qrcodeWidget=new QrcodeWidget(this);

    QString url=webServer->openReceiver(ui->ips_combox->currentText(),Settings::WebPort());
    qrcodeWidget->setUrl(url);

    layout->addWidget(qrcodeWidget);
    ui->qrcode_area->setLayout(layout);
    update();
}

void WebReceive::copyBtnClick()
{
    removeOld();
    havaQrcode=true;
    QString url=webServer->openReceiver(ui->ips_combox->currentText(),Settings::WebPort());
    QClipboard *clipboard = QApplication::clipboard();   //获取系统剪贴板指针
    clipboard->setText(url);
    QMessageBox::information(this,"提示","地址已复制到剪切板");
}


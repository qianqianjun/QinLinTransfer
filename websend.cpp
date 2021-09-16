#include "websend.h"
#include "ui_websend.h"
WebSend::WebSend(WebServer*& webServer,QWidget *parent) :
    QWidget(parent),
    ui(new Ui::websend),havaQrcode(false),webServer(webServer)
{
    ui->setupUi(this);
    this->setWindowModality(Qt::ApplicationModal);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowTitle("发送文件");
    ui->lineEdit->setEnabled(false);

    QStringList ips=getLocalHostIP();
    for(int i=0;i<ips.size();i++){
        ui->ip_combox->addItem(ips[i]);
    }
    connect(ui->selectFileBtn,&QPushButton::clicked,this,&WebSend::selectFileBtnClicked);
    connect(ui->generateCodeBtn,&QPushButton::clicked,this,&WebSend::generateCodeBtnClicked);
}

void WebSend::removeOld()
{
    if(havaQrcode){
        delete ui->qrcode_area->layout();
        delete qrcodeWidget;
    }
}

WebSend::~WebSend()
{
    delete ui;
}

void WebSend::selectFileBtnClicked()
{
    QStringList paths=QFileDialog::getOpenFileNames(this,"打开文件",QDir::homePath());
    if(!paths.empty()){
        QString str;
        for(int i=0;i<paths.size();i++){
            QFileInfo info(paths[i]);
            str.append(info.fileName()+" ");
        }
        ui->lineEdit->setText(str);
        filePaths=paths;
    }
}


void WebSend::generateCodeBtnClicked()
{
    if(filePaths.empty()){
        QMessageBox::critical(this,"错误","请选择文件");
        return;
    }
    removeOld();
    havaQrcode=true;
    QVBoxLayout *layout=new QVBoxLayout(this);
    qrcodeWidget=new QrcodeWidget(this);
    QString url=webServer->openSender(ui->ip_combox->currentText(),Settings::WebPort(),filePaths);
    qrcodeWidget->setUrl(url);
    layout->addWidget(qrcodeWidget);
    ui->qrcode_area->setLayout(layout);
    update();
}

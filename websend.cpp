#include "websend.h"
#include "ui_websend.h"
#include <QFile>
#include <QFileDialog>
#include <QString>
#include <QPainter>
#include <QBrush>
#include <QDebug>

websend::websend(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::websend)
{
    ui->setupUi(this);
    setWindowTitle("发送文件");
}

websend::~websend()
{
    delete ui;
}

void websend::on_pushButton_clicked()
{
    QString path=QFileDialog::getOpenFileName(this,"打开文件","./");
    ui->lineEdit->setText(path);
    _str=path.toUtf8();

}

void websend::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    QBrush brush(Qt::black);
    paint.setBrush(brush);
    QRcode* mqrcode=QRcode_encodeString(_str.data(),0,QR_ECLEVEL_Q,QR_MODE_8,true);
    if(mqrcode!=NULL){
        this->_size=(this->width()-50)/mqrcode->width;
        this->_margin=(this->width()/2)-(mqrcode->width*_size)/2;
        unsigned char* poin=mqrcode->data;
        for(int x=0;x<mqrcode->width;x++){
            for(int y=0;y<mqrcode->width;y++){
                if(*poin &1){
                    QRectF r(x*_size+_margin,y*_size+_margin,_size,_size);
                    paint.drawRect(r);
                }
                poin++;
            }
        }
    }
}


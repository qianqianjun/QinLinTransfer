#include "webreceive.h"
#include "ui_webreceive.h"
#include <QString>
#include <QPainter>
#include <QBrush>

webreceive::webreceive(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::webreceive)
{
    ui->setupUi(this);
    setWindowTitle("接收文件");
    _str="aaa";
}

webreceive::~webreceive()
{
    delete ui;
}

void webreceive::paintEvent(QPaintEvent *event)
{
    QPainter paint(this);
    QBrush brush(Qt::black);
    paint.setBrush(brush);
    QRcode* mqrcode=QRcode_encodeString(_str.data(),0,QR_ECLEVEL_Q,QR_MODE_8,true);
    this->_size=(this->width()-50)/mqrcode->width;
    this->_margin=(this->width()/2)-(mqrcode->width*_size)/2;
    unsigned char* poin=mqrcode->data;
    for(int x=0;x<mqrcode->width;x++){
        for(int y=0;y<mqrcode->width;y++){
            if(*poin&1){
                QRectF r(x*_size+_margin,y*_size+_margin,_size,_size);
                paint.drawRect(r);
            }
            poin++;
        }
    }
}

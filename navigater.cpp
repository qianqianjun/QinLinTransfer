#include "navigater.h"
#include <QDebug>

NavigaterLabel::~NavigaterLabel(){}

NavigaterLabel::NavigaterLabel(int index,QString text,QWidget* parent,Qt::WindowFlags f):QLabel(text,parent,f),index(index){}
void NavigaterLabel::mousePressEvent(QMouseEvent *event){
    emit clicked(this->index);
}

Navigater::Navigater(QStackedWidget* stackedWidget,QVector<NavigaterLabel*> menus,QObject* parent):QObject(parent),labels(menus),pages(stackedWidget),currentIndex(0){
    for(int i=0;i<this->labels.size();i++){
        connect(this->labels[i],&NavigaterLabel::clicked,this,&Navigater::selected);
    }
    pages->setCurrentIndex(0);
    labels[0]->setStyleSheet("QLabel{background-color:rgb(200,101,102);}");
}

void Navigater::selected(int index){
    if(currentIndex!=index){
        currentIndex=index;
        for(int i=0;i<labels.size();i++){
            labels[i]->setStyleSheet("QLabel{background-color:rgb(255,255,255);}");
        }
        labels[index]->setStyleSheet("QLabel{background-color:rgb(200,101,102);}");
        pages->setCurrentIndex(index);
    }
}

#ifndef WEBRECEIVE_H
#define WEBRECEIVE_H

#include <QWidget>
#include "qrencode/qrencode.h"

namespace Ui {
class webreceive;
}

class webreceive : public QWidget
{
    Q_OBJECT

public:
    explicit webreceive(QWidget *parent = nullptr);
    ~webreceive();

protected:
    void paintEvent(QPaintEvent *event);

private slots:

private:
    Ui::webreceive *ui;
    int _size;
    int _margin;
    QByteArray _str;
};

#endif // WEBRECEIVE_H

#ifndef WEBSEND_H
#define WEBSEND_H

#include <QWidget>
#include "qrencode/qrencode.h"


namespace Ui {
class websend;
}

class websend : public QWidget
{
    Q_OBJECT

public:
    explicit websend(QWidget *parent = nullptr);
    ~websend();

protected:
    void paintEvent(QPaintEvent *event);

private slots:
    void on_pushButton_clicked();
private:

    Ui::websend *ui;
    int _size;
    int _margin;
    QByteArray _str;
};

#endif // WEBSEND_H

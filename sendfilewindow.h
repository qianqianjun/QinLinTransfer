#ifndef SENDFILEWINDOW_H
#define SENDFILEWINDOW_H
#include <QDialog>
#include <onlinedeviceitem.h>
#include <QHeaderView>
#include <QMessageBox>
namespace Ui {
class sendFileWindow;
}

typedef struct finfo{
    QString fileName;
    QString fileSize;
    finfo(QString name,QString size):fileName(name),fileSize(size){}
}FileInfo;
class SendFileManager:public QObject{
    Q_OBJECT
public:
    QVector<FileInfo> files;
    int selectedIndex;
    SendFileManager(QObject* parent=nullptr);
public slots:
    void changeIndex(int row,int col);
};

class SendFileWindow : public QDialog{
    Q_OBJECT
private:
    QVector<QString> names;
    QVector<QString> ips;
    SendFileManager* manager;
public:
    explicit SendFileWindow(QVector<QString> names,
                            QVector<QString> ips,QWidget *parent = nullptr);
    ~SendFileWindow();
public slots:

private:
    Ui::sendFileWindow *ui;
};

#endif // SENDFILEWINDOW_H

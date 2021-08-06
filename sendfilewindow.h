#ifndef SENDFILEWINDOW_H
#define SENDFILEWINDOW_H
#include <QDialog>
#include <onlinedeviceitem.h>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QList>
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
    QList<FileInfo> fileInfos;
    QList<QSharedPointer<QFile>> files;
    int selectedIndex;
    SendFileManager(QObject* parent=nullptr);
    void addFile(const QString& filename);
    void removeFile();
    QString parseSize(qint64 size);
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
    void initTargetDevice();
    void initSelectedFileArea();
    void renderselectedFiles();
public slots:
private:
    Ui::sendFileWindow *ui;
};
#endif // SENDFILEWINDOW_H

#ifndef SENDFILEWINDOW_H
#define SENDFILEWINDOW_H
#include <QDialog>
#include <onlinedeviceitem.h>
#include <QHeaderView>
#include <QMessageBox>
#include <QFileDialog>
#include <QList>
#include <QMessageBox>
#include <QTcpSocket>
#include <QTimer>
#include <util.h>
#include <filetransfersender.h>
#include <filetransferdialog.h>

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
    QVector<DeviceInfo> targetDevices;
    QList<QSharedPointer<QFile>> files;
    QTcpSocket *socket;
    QTimer socketTimeoutTimer;
    int selectedIndex;

    SendFileManager(QVector<DeviceInfo> targetDevices,QObject* parent=nullptr);
    void addFile(const QString& filename);
    void removeFile();
    QString parseSize(qint64 size);

signals:
    void closeSelectFileWindow(int code);
    void changeBtnAble(bool enable);
public slots:
    void changeIndex(int row,int col);
    void socketConnected();
    void socketErrorOccurred();
    void socketTimeout();
    void sendFile();
};

class SendFileWindow : public QDialog{
    Q_OBJECT
private:
    QVector<DeviceInfo> targetDevices;
    SendFileManager* manager;
public:
    explicit SendFileWindow(QVector<DeviceInfo> infos,QWidget *parent = nullptr);
    ~SendFileWindow();
    void initTargetDevice();
    void initSelectedFileArea();
    void renderselectedFiles();
public slots:
private:
    Ui::sendFileWindow *ui;
};
#endif // SENDFILEWINDOW_H

#ifndef MAINUI_H
#define MAINUI_H

#include <QMainWindow>
#include <navigater.h>
#include <QLayout>
#include <QPushButton>
#include <onlinedeviceitem.h>
#include <QSpacerItem>
namespace Ui {
class MainUI;
}

class MainUI : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainUI(QWidget *parent = nullptr);
    ~MainUI();

private:
    Navigater* navigater;
    Ui::MainUI *ui;
    DeviceManager* manager;
    void initialLeftMenu();
    void initialOnlinePage(const QVector<DeviceInfo>& devices);
};

#endif // MAINUI_H

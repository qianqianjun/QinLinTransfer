#ifndef MAINUI_H
#define MAINUI_H

#include <QMainWindow>

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
    Ui::MainUI *ui;
};

#endif // MAINUI_H

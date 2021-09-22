#ifndef MANAGEWINDOW_H
#define MANAGEWINDOW_H

#include <QDialog>
#include <util.h>
#include <QMenu>
#include <QMessageBox>
namespace Ui {
class ManageWindow;
}

class ManageWindow : public QDialog
{
    Q_OBJECT

public:
    ManageWindow(QList<FileInfo>& fileInfos,QWidget* parent=nullptr);
    void renderDir();
    void deleteDir(QList<int>& Dels,VirtualFile* VF);
    ~ManageWindow();
signals:
    void saveSetting(QList<int> deleteIndexs);
private:
    VirtualFile* root;
    VirtualFile* currentDir;
    QMenu* menu;
    Ui::ManageWindow *ui;
    QList<int> deleteIndexs;
public slots:
    void goToFatherDir();
    void gotoSubDir(int row,int col);
    void showMenu(const QPoint pos);
    void removeItem();
};

#endif // MANAGEWINDOW_H

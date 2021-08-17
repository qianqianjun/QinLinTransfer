#ifndef PROGRESSBARUI_H
#define PROGRESSBARUI_H

#include <QDialog>
#include <context.h>
#include <QMessageBox>
#include <util.h>
#include <QListWidget>
#include <QListWidgetItem>
namespace Ui {
class Scheduler;
}

class ProgressBarUI : public QDialog
{
    Q_OBJECT
private:
    Ui::Scheduler *ui;
    Context *context;
    bool errored;
    QMessageBox confirmBox;
private slots:
    void response(int result);
    void updateBar(double progress);
    void errorAppear(const QString& err);
    void openConfirmDialog(const QVector<FileMetadata> &metaData,quint64 totalSize,
                           const QString &deviceName,const QString &keyDigest);
    void printLog(const QString msg);
protected:
    void closeEvent(QCloseEvent* event);
public:
    explicit ProgressBarUI(Context *context,QWidget *parent = nullptr);
    ~ProgressBarUI();
signals:
    void transferInterruptedByUser();
};

#endif // PROGRESSBARUI_H

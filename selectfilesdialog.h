#pragma once
#include <QDialog>
#include <QFile>
#include <QStringListModel>
#include "discoveryservice.h"
namespace Ui {
    class SelectFilesDialog;
}
class SelectFilesDialog : public QDialog {
    Q_OBJECT
public:
    explicit SelectFilesDialog(QWidget *parent, DiscoveryService &discoveryService);
    ~SelectFilesDialog();
private:
    Ui::SelectFilesDialog *ui;
    DiscoveryService &discoveryService;
    QList<QSharedPointer<QFile>> files;
    QStringListModel filesStringListModel;
    void addFile(const QString &filename);
    void updateFileStringListModel();
private slots:
    void addButtonClicked();
    void removeButtonClicked();
    void accept();
protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);
};
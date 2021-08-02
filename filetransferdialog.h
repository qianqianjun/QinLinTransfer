#pragma once

#include <QDialog>
#include <QFile>
#include <QMessageBox>
#include <QTcpSocket>

#include "filetransfersession.h"

namespace Ui {
    class FileTransferDialog;
}

class FileTransferDialog : public QDialog {
    Q_OBJECT
public:
    explicit FileTransferDialog(QWidget *parent, FileTransferSession *session);
    ~FileTransferDialog();
private:
    Ui::FileTransferDialog *ui;
    FileTransferSession *session; // 父类的指针指向子类的对象。
    bool errored;
    QMessageBox questionBox;
private slots:
    void respond(int result);
    void sessionUpdateProgress(double progress);
    void sessionErrorOccurred(const QString &msg);
    void sessionFileMetadataReady(const QList<FileTransferSession::FileMetadata> &metadata, quint64 totalSize,
                                  const QString &deviceName, const QString &sessionKeyDigest);
};

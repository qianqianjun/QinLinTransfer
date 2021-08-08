#include <QAbstractButton>

#include "filetransferdialog.h"
#include "ui_filetransferdialog.h"

// 这里是传送的进度条实现
FileTransferDialog::FileTransferDialog(QWidget *parent, FileTransferSession *session) :
    QDialog(parent), ui(new Ui::FileTransferDialog), session(session), errored(false), questionBox(this)
{
    qDebug()<<"FileTransferDialog::FileTransferDialog";
    ui->setupUi(this);
    setWindowFlag(Qt::WindowStaysOnTopHint); // 告知窗口系统，该窗口应该停留在所有其他窗口的上面。

    // questionBox 被设置为当前对象。
    questionBox.setIcon(QMessageBox::Question);
    questionBox.setWindowTitle(QApplication::applicationName());
    questionBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    questionBox.setDefaultButton(QMessageBox::Yes);
    // finish是QMessageBox的父类 QDialog的方法，当dialog的result code被设置的时候会被触发。
    // 也就是执行 done，accept或者reject之后被触发。
    // done中可以设置参数，达到和accept或reject一样的效果
    // finish状态被触发后，执行 respond函数（一个私有的槽函数）
    connect(&questionBox, &QMessageBox::finished, this, &FileTransferDialog::respond);

    // session 类型为 filetransferssion，继承自 QObject
    session->setParent(this);
    connect(session, &FileTransferSession::printMessage, ui->statusLabel, &QLabel::setText);
    connect(session, &FileTransferSession::updateProgress, this, &FileTransferDialog::sessionUpdateProgress);
    connect(session, &FileTransferSession::errorOccurred, this, &FileTransferDialog::sessionErrorOccurred);
    connect(session, &FileTransferSession::fileMetadataReady, this, &FileTransferDialog::sessionFileMetadataReady);

    // 传送结束-> session.end -> 进度条窗口关闭
    connect(session, &FileTransferSession::ended, this, &FileTransferDialog::accept); // accept 为QDialog组件自带。
    session->start(); // 两个操作：（1）触发信号printMessage，显示正在握手，（2）发送公钥
}

FileTransferDialog::~FileTransferDialog()
{
    delete ui;
}

void FileTransferDialog::respond(int result)
{
    qDebug()<<"FileTransferDialog::respond running!";
    bool response = result == QMessageBox::Yes;
    session->respond(response); // 不知道为什么会执行这一步
    if (!response) // response 不是Yes
        hide(); // 隐藏窗口
}

void FileTransferDialog::sessionUpdateProgress(double progress)
{
    qDebug()<<"FileTransferDialog::sessionUpdateProgress";
    ui->progressBar->setValue(ui->progressBar->maximum() * progress);
}

void FileTransferDialog::sessionErrorOccurred(const QString &msg)
{
    qDebug()<<"FileTransferDialog::sessionErrorOccurred --start";
    qDebug()<<errored;
    qDebug()<<"visable: "<<isVisible();
    qDebug()<<msg;
    if (errored)
        return;
    errored = true;
    if (isVisible())
        QMessageBox::critical(this, QApplication::applicationName(), msg);
    done(QDialog::Rejected);
    qDebug()<<"FileTransferDialog::sessionErrorOccurred --end";
}

// 这是接收端的函数
void FileTransferDialog::sessionFileMetadataReady(const QList<FileTransferSession::FileMetadata> &metadata,
                                                  quint64 totalSize,
                                                  const QString &deviceName,
                                                  const QString &sessionKeyDigest)
{
    qDebug()<<"FileTransferDialog::sessionFileMetadataReady";
    show();
    QString totalSizeStr = locale().formattedDataSize(totalSize, 2, QLocale::DataSizeTraditionalFormat);
    QString msg;
    if (metadata.size() == 1) {
        msg = QString("%1 向您分享文件： \"%2\" 大小为： %3。")
                .arg(deviceName, metadata.first().filename, totalSizeStr);
    } else {
        msg = QString("%1 向您分享 %2 个文件，总大小为 %3.")
                .arg(deviceName).arg(metadata.size()).arg(totalSizeStr);
    }
    msg += QString("\n确认在发送设备上显示了验证码： \"%1\" 。").arg(sessionKeyDigest);
    msg += QString("\n是否接收文件？");
    questionBox.setText(msg);
    questionBox.show();
}

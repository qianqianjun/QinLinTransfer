#include <QDragEnterEvent>
#include <QDropEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>

#include "selectfilesdialog.h"
#include "sendtodialog.h"
#include "ui_selectfilesdialog.h"

SelectFilesDialog::SelectFilesDialog(QWidget *parent, DiscoveryService &discoveryService) :
    QDialog(parent), ui(new Ui::SelectFilesDialog), discoveryService(discoveryService)
{
    ui->setupUi(this);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    connect(ui->addButton, &QPushButton::clicked, this, &SelectFilesDialog::addButtonClicked);
    connect(ui->removeButton, &QPushButton::clicked, this, &SelectFilesDialog::removeButtonClicked);
    ui->filesListView->setModel(&filesStringListModel);

    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Send"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
}

SelectFilesDialog::~SelectFilesDialog()
{
    delete ui;
}

void SelectFilesDialog::addFile(const QString &filename)
{
    foreach (QSharedPointer<QFile> file, files) {
        if (file->fileName() == filename)
            return;
    }

    QSharedPointer<QFile> fp = QSharedPointer<QFile>::create(filename);
    if (!fp->open(QIODevice::ReadOnly)) {
        QMessageBox::critical(this, QApplication::applicationName(),
                              tr("Unable to open file %1. Skipping.")
                              .arg(filename));
        return;
    }
    if (fp->isSequential()) {
        QMessageBox::critical(this, QApplication::applicationName(),
                              tr("%1 is not a regular file. Skipping.")
                              .arg(filename));
        return;
    }
    files.append(fp);
}

void SelectFilesDialog::updateFileStringListModel()
{
    QStringList l;
    foreach (QSharedPointer<QFile> file, files) {
        l.append(QFileInfo(*file).fileName());
    }
    filesStringListModel.setStringList(l);
}

void SelectFilesDialog::addButtonClicked()
{
    QStringList filenames = QFileDialog::getOpenFileNames(nullptr, tr("Select File(s) to be Sent"));
    if (filenames.empty())
        return;

    foreach (const QString &filename, filenames) {
        addFile(filename);
    }

    updateFileStringListModel();
}

void SelectFilesDialog::removeButtonClicked()
{
    QModelIndexList indexes = ui->filesListView->selectionModel()->selectedIndexes();
    QList<const QSharedPointer<QFile> *> removeList;
    foreach (const QModelIndex &i, indexes) {
        removeList.append(&files.at(i.row()));
    }
    foreach (const QSharedPointer<QFile> *fp, removeList) {
        files.removeOne(*fp);
    }
    updateFileStringListModel();
}

void SelectFilesDialog::accept()
{
    if (files.empty()) {
        QMessageBox::warning(this, QApplication::applicationName(), tr("No file to be sent."));
        return;
    }

    SendToDialog *d = new SendToDialog(nullptr, files, discoveryService);
    d->setAttribute(Qt::WA_DeleteOnClose);
    d->show();

    done(Accepted);
}

void SelectFilesDialog::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
        event->acceptProposedAction();
}

void SelectFilesDialog::dropEvent(QDropEvent *event)
{
    if (event->mimeData()->hasUrls()) {
        foreach (const QUrl &url, event->mimeData()->urls()) {
            addFile(url.toLocalFile());
        }
        updateFileStringListModel();
        event->acceptProposedAction();
    }
}
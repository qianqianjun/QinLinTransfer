#include "managewindow.h"
#include "ui_managewindow.h"

ManageWindow::ManageWindow(QList<FileInfo> &fileInfos, QWidget *parent):
    QDialog(parent),ui(new Ui::ManageWindow)
{
    ui->setupUi(this);
    setWindowTitle("管理发送文件");
    setWindowFlag(Qt::WindowStaysOnTopHint);
    setWindowModality(Qt::ApplicationModal);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);

    // 构建目录树
    root=new VirtualFile(nullptr,this);
    root->pwd="/";
    for(int key=0;key<fileInfos.size();key++){
        QList<QString> parts=fileInfos[key].fileName.split("/");
        root->addFile(parts,key,fileInfos[key]);
    }
    // 设置当前所在目录
    currentDir=root;
    // 设置表格事件绑定
    ui->tableWidget->setSelectionBehavior ( QAbstractItemView::SelectRows); //设置选择行为，以行为单位
    ui->tableWidget->setSelectionMode ( QAbstractItemView::SingleSelection); //设置选择模式，选择单行
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->setShowGrid(false);
    ui->tableWidget->verticalHeader()->setHidden(true);
    connect(ui->tableWidget,&QTableWidget::cellDoubleClicked,this,&ManageWindow::gotoSubDir);
    // 右键菜单
    menu=new QMenu(ui->tableWidget);
    QAction* action=new QAction("删除",ui->tableWidget);
    connect(action,&QAction::triggered,this,&ManageWindow::removeItem);
    menu->addAction(action);
    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget,&QTableWidget::customContextMenuRequested,this,&ManageWindow::showMenu);
    // 渲染目录
    renderDir();
    connect(ui->up_btn,&QPushButton::clicked,this,&ManageWindow::goToFatherDir);

    // 为按钮设置绑定事件
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("保存");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    connect(this,&ManageWindow::accepted,[=](){
        emit saveSetting(deleteIndexs);
    });
}

void ManageWindow::renderDir()
{
    ui->tableWidget->clearContents();
    // 设置目录的位置
    ui->address->setText(currentDir->pwd);
    // 将目录树可视化展示
    ui->tableWidget->setRowCount(currentDir->dirs.size()+currentDir->files.size());
    int row=0;
    int i;
    for(i=0;i<currentDir->dirs.size();i++){
        int col=0;
        QString key=currentDir->dirs.keys().at(i);
        QTableWidgetItem* item=new QTableWidgetItem(key);
        item->setIcon(QIcon(":/icons/dir.png"));
        ui->tableWidget->setItem(row,col++,item);
        ui->tableWidget->setItem(row,col++,new QTableWidgetItem("文件夹"));
        ui->tableWidget->setItem(row,col++,new QTableWidgetItem(""));
        row++;
    }
    for(int j=0;j<currentDir->files.size();j++){
        int col=0;
        QFileInfo temp(currentDir->files[j]->info.fileName);
        QTableWidgetItem* item=new QTableWidgetItem(temp.fileName());
        item->setIcon(QIcon(":/icons/file.png"));
        ui->tableWidget->setItem(row,col++,item);
        ui->tableWidget->setItem(row,col++,new QTableWidgetItem("文件"));
        ui->tableWidget->setItem(row,col++,new QTableWidgetItem(currentDir->files[j]->info.fileSize));
        row++;
    }
}

ManageWindow::~ManageWindow()
{
    delete ui;
}

void ManageWindow::goToFatherDir()
{
    VirtualFile* father=currentDir->father;
    if(father==nullptr){
        QMessageBox::critical(this,"错误","已经是根目录了！");
    }else{
        currentDir=father;
        renderDir();
    }
}

void ManageWindow::gotoSubDir(int row,int col)
{
    col++;
    if(row<currentDir->dirs.size()){
        QString key=currentDir->dirs.keys().at(row);
        currentDir=currentDir->dirs.value(key);
        renderDir();
    }
}

void ManageWindow::showMenu(const QPoint pos){
    QModelIndex index=ui->tableWidget->indexAt(pos);
    if(index.row()!=-1){
        ui->tableWidget->setCurrentIndex(index);
        menu->move(cursor().pos());
        menu->show();
    }
}

void ManageWindow::deleteDir(QList<int> &Dels, VirtualFile* VF)
{
    for(int i=0;i<VF->files.size();i++){
        Dels.append(VF->files.at(i)->key);
    }
    for(int i=0;i<VF->dirs.size();i++){
        QString key=VF->dirs.keys().at(i);
        deleteDir(Dels,VF->dirs.value(key));
    }
}

void ManageWindow::removeItem()
{
    QModelIndex pos=ui->tableWidget->currentIndex();
    int row=pos.row();
    if(row<currentDir->dirs.size()){ // 删除一个目录
        QString key=currentDir->dirs.keys().at(row);
        deleteDir(deleteIndexs,currentDir->dirs.value(key));
        currentDir->dirs.remove(key);
        renderDir();
        return;
    }
    row-=currentDir->dirs.size();
    if(row<currentDir->files.size()){ // 删除一个文件
        deleteIndexs.append(currentDir->files.at(row)->key);
        currentDir->files.removeAt(row);
        renderDir();
    }
}

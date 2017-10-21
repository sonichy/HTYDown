#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "form.h"
#include "ui_form.h"
#include <QDesktopWidget>
#include <QMessageBox>
#include <QFileDialog>
#include <QDateTime>
#include <QtNetwork>
#include <QElapsedTimer>
#include <QDesktopServices>
#include <QClipboard>
#include <QCheckBox>
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->listWidgetDownloaded->hide();
    ui->listWidgetTrash->hide();
    ui->listWidgetNav->addItem(new QListWidgetItem(QIcon(":/images/down.png"), "正在下载"));
    ui->listWidgetNav->addItem(new QListWidgetItem(QIcon(":/images/tick.png"), "已经下载"));
    ui->listWidgetNav->addItem(new QListWidgetItem(QIcon(":/images/trash.png"), "垃圾箱"));
    ui->listWidgetNav->setCurrentRow(0);

    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);

    dialognew = new DialogNew(this);
    connect(dialognew,SIGNAL(accepted()),this,SLOT(addnew()));
    connect(ui->actionNew,SIGNAL(triggered(bool)),this,SLOT(showDialogNew()));
    connect(ui->action_new,SIGNAL(triggered(bool)),this,SLOT(showDialogNew()));
    connect(ui->listWidgetNav,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(itemClick(QListWidgetItem*)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showDialogNew()
{
    dialognew->show();
    QClipboard *clipboard = QApplication::clipboard();
    QByteArray BA = QByteArray::fromPercentEncoding(clipboard->text().toUtf8());
    QString s = BA;
    dialognew->ui->lineEditURL->setText(s);
    dialognew->ui->lineEditURL->setCursorPosition(0);
    dialognew->getFilename("");
}

void MainWindow::on_action_quit_triggered()
{
    qApp->quit();
}

void MainWindow::on_action_changelog_triggered()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "更新历史", "1.0\n2017-10\n增加左侧导航栏，暂无功能。\n修复空列表打开文件夹崩溃Bug。\n使用fromPercentEncoding还原剪贴板网址编码。\n2017-06\n自定义列表行封装下载请求，解决下载互相干扰的问题。\n修复数据溢出问题。\n从剪贴板读取下载地址。\n从主程序中分离新建下载的部分方法。\n新建填入默认下载目录。\n\n0.1\n2017-01\n增加打开下载目录。\n添加行，删除行。\n增加停止下载。\n增加下载时长，下载字节单位换算。\n加入新建下载。\n制作主界面和新建界面。");
    aboutMB.exec();
}

void MainWindow::on_action_aboutQt_triggered()
{
    QMessageBox::aboutQt(NULL, "关于 Qt");
}

void MainWindow::on_action_about_triggered()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "海天鹰下载 1.0\n一款基于Qt的下载程序。\n作者：黄颖\nE-mail: sonichy@163.com\n主页：sonichy.96.lt\n参考文献：\n速度、时间计算，字节单位换算：http://blog.csdn.net/liang19890820/article/details/50814339");
    QPixmap pixmap;
    pixmap.load(":images/icon.png");
    aboutMB.setIconPixmap(pixmap);
    aboutMB.exec();
}

void MainWindow::on_actionStart_triggered()
{

}

void MainWindow::on_actionPause_triggered()
{
    if(ui->listWidgetDownloading->count() != 0){
        if(((Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(ui->listWidgetDownloading->currentRow()))))->reply){
            if(!((Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(ui->listWidgetDownloading->currentRow()))))->reply->isFinished())((Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(ui->listWidgetDownloading->currentRow()))))->reply->abort();
            qDebug() << "pause reply" << ((Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(ui->listWidgetDownloading->currentRow()))))->reply;
        }
    }
}

void MainWindow::on_actionDelete_triggered()
{
    //on_actionPause_triggered();    
    if(ui->listWidgetNav->currentRow() == 0){
        Form *form = (Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(ui->listWidgetDownloading->currentRow())));
        QListWidgetItem *LWI = new QListWidgetItem(ui->listWidgetTrash);
        LWI->setSizeHint(QSize(1200,30));
        ui->listWidgetTrash->setItemWidget(LWI,form);
        ui->listWidgetTrash->addItem(LWI);
        ui->listWidgetDownloading->setCurrentRow(-1);
    }else if(ui->listWidgetNav->currentRow() == 1){
        Form *form = (Form*)(ui->listWidgetDownloaded->itemWidget(ui->listWidgetDownloaded->item(ui->listWidgetDownloaded->currentRow())));
        QListWidgetItem *LWI = new QListWidgetItem(ui->listWidgetTrash);
        ui->listWidgetTrash->addItem(LWI);
        ui->listWidgetTrash->setItemWidget(LWI,form);
        LWI->setSizeHint(QSize(1200,30));        
    }else if(ui->listWidgetNav->currentRow() == 2){
        Form *form = (Form*)(ui->listWidgetTrash->itemWidget(ui->listWidgetTrash->item(ui->listWidgetTrash->currentRow())));
        QMessageBox msgbox(QMessageBox::Critical, "删除", "你确定要删除此任务吗？", QMessageBox::Ok | QMessageBox::Cancel);
        QCheckBox checkDelete("同时删除文件", &msgbox);
        msgbox.layout()->addWidget(&checkDelete);
        if(msgbox.exec() == QMessageBox::Ok){
            bool b = checkDelete.checkState();
            qDebug() << b;
            if(b){
                QString filename = form->ui->labelPath->text() + "/" + form->ui->labelFilename->text();
                qDebug() << filename;
                QFile::remove(filename);
            }
            ui->listWidgetTrash->takeItem(ui->listWidgetTrash->currentRow());
        }
    }
}

void MainWindow::on_actionDirectory_triggered()
{    
    if(ui->listWidgetDownloading->count() != 0){
        QString path=((Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(ui->listWidgetDownloading->currentRow()))))->ui->labelPath->text();
        qDebug() << path;
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
}

void MainWindow::addnew()
{
    QDateTime time = QDateTime::currentDateTime();
    QString stime = time.toString("yyyy-MM-dd hh:mm:ss");
    QString surl = dialognew->ui->lineEditURL->text();
    QString sfn = dialognew->ui->lineEditFilename->text();
    QString spath = dialognew->ui->lineEditPath->text();
    Form *form = new Form;
    form->ui->labelFilename->setText(sfn);
    form->ui->labelURL->setText(surl);
    form->ui->labelURL->adjustSize();
    form->ui->labelPath->setText(spath);
    form->ui->labelTimeCreate->setText(stime);
    QListWidgetItem *LWI = new QListWidgetItem(ui->listWidgetDownloading);
    ui->listWidgetDownloading->addItem(LWI);
    ui->listWidgetDownloading->setItemWidget(LWI,form);
    LWI->setSizeHint(QSize(1200,30));
    form->download(surl);
    connect(form,SIGNAL(downloadFinish()),this,SLOT(moveToDownloaded()));
}

void MainWindow::itemClick(QListWidgetItem *item)
{
    Q_UNUSED(item);
    //qDebug() << item->text();
    switch( ui->listWidgetNav->currentRow() ) {
    case 0:
        ui->listWidgetDownloading->show();
        ui->listWidgetDownloaded->hide();
        ui->listWidgetTrash->hide();
        break;
    case 1:
        ui->listWidgetDownloading->hide();
        ui->listWidgetDownloaded->show();
        ui->listWidgetTrash->hide();
        break;
    case 2:
        ui->listWidgetDownloading->hide();
        ui->listWidgetDownloaded->hide();
        ui->listWidgetTrash->show();
        break;
    }
}

void MainWindow::moveToDownloaded()
{
    qDebug() << "moveToDownloaded";
    Form *form = qobject_cast<Form*>(sender());
    QListWidgetItem *LWI = new QListWidgetItem(ui->listWidgetDownloaded);
    ui->listWidgetDownloaded->addItem(LWI);
    ui->listWidgetDownloaded->setItemWidget(LWI,form);
    LWI->setSizeHint(QSize(1200,30));
    for(int i=0; i<ui->listWidgetDownloading->count(); i++){
        if( form == ((Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(i)))) ){
            ui->listWidgetDownloading->takeItem(i);
        }
    }
}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialognew.h"
#include "ui_dialognew.h"
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

DialogNew *dialognew;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QListWidgetItem *LWI1,*LWI2,*LWI3;
    LWI1 = new QListWidgetItem(QIcon(":/images/down.png"), "正在下载");
    LWI2 = new QListWidgetItem(QIcon(":/images/tick.png"), "已经下载");
    LWI3 = new QListWidgetItem(QIcon(":/images/trash.png"), "垃圾箱");
    ui->listWidgetNav->insertItem(1, LWI1);
    ui->listWidgetNav->insertItem(2, LWI2);
    ui->listWidgetNav->insertItem(3, LWI3);

    QDesktopWidget* desktop = QApplication::desktop();
    move((desktop->width() - this->width())/2, (desktop->height() - this->height())/2);

    dialognew = new DialogNew(this);
    connect(dialognew,SIGNAL(accepted()),this,SLOT(addnew()));
    connect(ui->actionNew,SIGNAL(triggered(bool)),this,SLOT(showDialogNew()));
    connect(ui->action_new,SIGNAL(triggered(bool)),this,SLOT(showDialogNew()));
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
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "海天鹰下载器 1.0\n一款基于Qt的下载程序。\n作者：黄颖\nE-mail: sonichy@163.com\n主页：sonichy.96.lt\n参考文献：\n速度、时间计算，字节单位换算：http://blog.csdn.net/liang19890820/article/details/50814339");
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
    if(ui->listWidget->count() != 0){
        if(((Form*)(ui->listWidget->itemWidget(ui->listWidget->item(ui->listWidget->currentRow()))))->reply){
            if(!((Form*)(ui->listWidget->itemWidget(ui->listWidget->item(ui->listWidget->currentRow()))))->reply->isFinished())((Form*)(ui->listWidget->itemWidget(ui->listWidget->item(ui->listWidget->currentRow()))))->reply->abort();
            qDebug() << "pause reply" << ((Form*)(ui->listWidget->itemWidget(ui->listWidget->item(ui->listWidget->currentRow()))))->reply;
        }
    }
}

void MainWindow::on_actionDelete_triggered()
{
    on_actionPause_triggered();
    ui->listWidget->takeItem(ui->listWidget->currentRow());
}

void MainWindow::on_actionDirectory_triggered()
{    
    if(ui->listWidget->count() != 0){
        QString path=((Form*)(ui->listWidget->itemWidget(ui->listWidget->item(ui->listWidget->currentRow()))))->ui->labelPath->text();
        qDebug() << path;
        QDesktopServices::openUrl(QUrl::fromLocalFile(path));
    }
}

void MainWindow::addnew()
{
    QDateTime time = QDateTime::currentDateTime();
    QString stime = time.toString("yyyy-MM-dd hh:mm:ss");
    QString surl = dialognew->ui->lineEditURL->text();
    QString sfn=dialognew->ui->lineEditFilename->text();
    QString spath=dialognew->ui->lineEditPath->text();
    Form *form=new Form;
    form->ui->labelFilename->setText(sfn);
    form->ui->labelURL->setText(surl);
    form->ui->labelURL->adjustSize();
    form->ui->labelPath->setText(spath);
    form->ui->labelTimeCreate->setText(stime);
    QListWidgetItem *LWI = new QListWidgetItem(ui->listWidget);
    ui->listWidget->addItem(LWI);
    ui->listWidget->setItemWidget(LWI,form);
    LWI->setSizeHint(QSize(1200,30));
    form->download(surl);
}

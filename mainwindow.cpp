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
#include <QTextBrowser>
#include <QPushButton>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    move((QApplication::desktop()->width() - width())/2, (QApplication::desktop()->height() - height())/2);
    ui->listWidgetDownloaded->hide();
    ui->listWidgetTrash->hide();
    ui->listWidgetNav->addItem(new QListWidgetItem(QIcon(":/images/down.png"), "正在下载"));
    ui->listWidgetNav->addItem(new QListWidgetItem(QIcon(":/images/tick.png"), "已经下载"));
    ui->listWidgetNav->addItem(new QListWidgetItem(QIcon(":/images/trash.png"), "垃圾箱"));
    ui->listWidgetNav->setCurrentRow(0);
    connect(ui->listWidgetDownloading, SIGNAL(customContextMenuRequested(QPoint)),this, SLOT(viewContextMenu(QPoint)));    

    dialogNew = new DialogNew(this);
    connect(dialogNew,SIGNAL(accepted()),this,SLOT(addNew()));
    connect(ui->actionNew,SIGNAL(triggered(bool)),this,SLOT(showDialogNew()));
    connect(ui->action_new,SIGNAL(triggered(bool)),this,SLOT(showDialogNew()));
    connect(ui->listWidgetNav,SIGNAL(itemClicked(QListWidgetItem*)),this,SLOT(itemClick(QListWidgetItem*)));

    //3个列表文件，不存在则创建文件，存在则加载文件。
    QString filename = "downloading";
    QString filepath = QDir::currentPath() + "/" + filename;
    QFile *file = new QFile(filepath);
    if (!file->exists()) {
        file->open(QIODevice::WriteOnly);
        file->close();
    } else {
        loadList(filename);
    }

    filename = "downloaded";
    filepath = QDir::currentPath() + "/" + filename;
    file->setFileName(filepath);
    if (!file->exists()) {
        file->open(QIODevice::WriteOnly);
        file->close();
    } else {
        loadList(filename);
    }

    filename = "trash";
    filepath = QDir::currentPath() + "/" + filename;
    file->setFileName(filepath);
    if (!file->exists()) {
        file->open(QIODevice::WriteOnly);
        file->close();
    } else {
        loadList(filename);
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showDialogNew()
{
    dialogNew->show();
    QClipboard *clipboard = QApplication::clipboard();
    QByteArray BA = QByteArray::fromPercentEncoding(clipboard->text().toUtf8());
    QString s = BA;
    if (s.contains("://")) {
        dialogNew->ui->textEditURL->setText(s);
        dialogNew->ui->textEditURL->textCursor().movePosition(QTextCursor::Start,QTextCursor::MoveAnchor,1);
    }
}

void MainWindow::on_action_quit_triggered()
{
    qApp->quit();
}

void MainWindow::on_action_changelog_triggered()
{
    QString s = "1.3\n2018-04\n使用QLineEdit内部按钮代替额外的按钮。\n\n1.2\n2018-01\n增加批量下载。\n增加一键清空。\n新建下载判断路径存在、可写。\n\n1.1\n2017-12\n增加迅雷协议解析。\n\n1.0\n2017-11\n更新日志使用QTextBrowser代替QMessageBox。\n采用逐个赋值的方法复制移动列表项，解决崩溃问题。\n2017-10\n保存正在下载、已经下载、垃圾箱列表。\n左侧增加正在下载、已经下载、垃圾箱。\n修复空列表打开文件夹崩溃Bug。\n使用fromPercentEncoding还原剪贴板网址编码。\n2017-06\n自定义列表行封装下载请求，解决下载互相干扰的问题。\n修复数据溢出问题。\n从剪贴板读取下载地址。\n从主程序中分离新建下载的部分方法。\n新建填入默认下载目录。\n\n0.1\n2017-01\n增加打开下载目录。\n添加行，删除行。\n增加停止下载。\n增加下载时长，下载字节单位换算。\n加入新建下载。\n制作主界面和新建界面。";
    QDialog *dialog = new QDialog;
    dialog->setWindowTitle("更新历史");
    dialog->setFixedSize(400,300);
    QVBoxLayout *vbox = new QVBoxLayout;
    QTextBrowser *textBrowser = new QTextBrowser;
    textBrowser->setText(s);
    textBrowser->zoomIn();
    vbox->addWidget(textBrowser);
    QHBoxLayout *hbox = new QHBoxLayout;
    QPushButton *btnConfirm = new QPushButton("确定");
    hbox->addStretch();
    hbox->addWidget(btnConfirm);
    hbox->addStretch();
    vbox->addLayout(hbox);
    dialog->setLayout(vbox);
    dialog->show();
    connect(btnConfirm, SIGNAL(clicked()), dialog, SLOT(accept()));
    if (dialog->exec() == QDialog::Accepted) {
        dialog->close();
    }
}

void MainWindow::on_action_aboutQt_triggered()
{
    QMessageBox::aboutQt(NULL, "关于 Qt");
}

void MainWindow::on_action_about_triggered()
{
    QMessageBox aboutMB(QMessageBox::NoIcon, "关于", "海天鹰下载 1.2\n一款基于Qt的下载程序。\n作者：黄颖\nE-mail: sonichy@163.com\n主页：sonichy.96.lt\n参考文献：\n速度、时间计算，字节单位换算：http://blog.csdn.net/liang19890820/article/details/50814339");
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
    if (ui->listWidgetDownloading->count() != 0) {
        if (((Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(ui->listWidgetDownloading->currentRow()))))->reply) {
            if(!((Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(ui->listWidgetDownloading->currentRow()))))->reply->isFinished())((Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(ui->listWidgetDownloading->currentRow()))))->reply->abort();
            qDebug() << "pause reply" << ((Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(ui->listWidgetDownloading->currentRow()))))->reply;
        }
    }
}

void MainWindow::on_actionDelete_triggered()
{
    //on_actionPause_triggered();
    if(ui->listWidgetNav->currentRow() == 0){
        if(ui->listWidgetDownloading->currentRow() != -1){
            Form *form = (Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(ui->listWidgetDownloading->currentRow())));
            Form *form1 = new Form;
            form1->ui->labelFilename->setText(form->ui->labelFilename->text());
            form1->ui->labelSize->setText(form->ui->labelSize->text());
            form1->ui->progressBar->setRange(0,form->ui->progressBar->maximum());
            form1->ui->progressBar->setValue(form->ui->progressBar->value());
            form1->ui->labelSpeed->setText(form->ui->labelSpeed->text());
            form1->ui->labelURL->setText(form->ui->labelURL->text());
            form1->ui->labelElapse->setText(form->ui->labelElapse->text());
            form1->ui->labelTimeCreate->setText(form->ui->labelTimeCreate->text());
            form1->ui->labelPath->setText(form->ui->labelPath->text());
            QListWidgetItem *LWI = new QListWidgetItem(ui->listWidgetTrash);
            LWI->setSizeHint(QSize(1200,30));
            ui->listWidgetTrash->setItemWidget(LWI,form1);
            ui->listWidgetTrash->addItem(LWI);
            ui->listWidgetDownloading->takeItem(ui->listWidgetDownloading->currentRow());
            ui->listWidgetDownloading->setCurrentRow(-1);
            saveList("downloading");
        }
    } else if (ui->listWidgetNav->currentRow() == 1) {
        if (ui->listWidgetDownloaded->currentRow() != -1) {
            Form *form = (Form*)(ui->listWidgetDownloaded->itemWidget(ui->listWidgetDownloaded->item(ui->listWidgetDownloaded->currentRow())));
            Form *form1 = new Form;
            form1->ui->labelFilename->setText(form->ui->labelFilename->text());
            form1->ui->labelSize->setText(form->ui->labelSize->text());
            form1->ui->progressBar->setRange(0,form->ui->progressBar->maximum());
            form1->ui->progressBar->setValue(form->ui->progressBar->value());
            form1->ui->labelSpeed->setText(form->ui->labelSpeed->text());
            form1->ui->labelURL->setText(form->ui->labelURL->text());
            form1->ui->labelElapse->setText(form->ui->labelElapse->text());
            form1->ui->labelTimeCreate->setText(form->ui->labelTimeCreate->text());
            form1->ui->labelPath->setText(form->ui->labelPath->text());
            QListWidgetItem *LWI = new QListWidgetItem(ui->listWidgetTrash);
            ui->listWidgetTrash->addItem(LWI);
            ui->listWidgetTrash->setItemWidget(LWI,form1);
            LWI->setSizeHint(QSize(1200,30));
            appendList("trash", form->ui->labelFilename->text(), form->ui->labelSize->text(), form->ui->progressBar->maximum(), form->ui->progressBar->value(), form->ui->labelSpeed->text(), form->ui->labelURL->text(), form->ui->labelElapse->text(), form->ui->labelTimeCreate->text(), form->ui->labelPath->text());
            ui->listWidgetDownloaded->takeItem(ui->listWidgetDownloaded->currentRow());
            ui->listWidgetDownloading->setCurrentRow(-1);
            saveList("downloaded");
        }
    } else if (ui->listWidgetNav->currentRow() == 2) {
        if (ui->listWidgetTrash->currentRow() != -1) {
            Form *form = (Form*)(ui->listWidgetTrash->itemWidget(ui->listWidgetTrash->item(ui->listWidgetTrash->currentRow())));
            QMessageBox msgbox(QMessageBox::Critical, "删除", "你确定要删除此任务吗？", QMessageBox::Ok | QMessageBox::Cancel);
            QCheckBox checkDelete("同时删除文件", &msgbox);
            msgbox.layout()->addWidget(&checkDelete);
            if (msgbox.exec() == QMessageBox::Ok) {
                bool b = checkDelete.checkState();
                qDebug() << b;
                if (b) {
                    QString filename = form->ui->labelPath->text() + "/" + form->ui->labelFilename->text();
                    qDebug() << filename;
                    QFile::remove(filename);
                }
                ui->listWidgetTrash->takeItem(ui->listWidgetTrash->currentRow());
                ui->listWidgetDownloading->setCurrentRow(-1);
                saveList("trash");
            }
        }
    }
}

void MainWindow::on_actionTrash_triggered()
{
    if (ui->listWidgetNav->currentRow() == 0) {
        QMessageBox msgbox(QMessageBox::Critical, "清空", "你确定要清空【正在下载】列表吗？", QMessageBox::Ok | QMessageBox::Cancel);
        if (msgbox.exec() == QMessageBox::Ok) {
            ui->listWidgetDownloading->clear();
            saveList("downloading");
        }
    } else if (ui->listWidgetNav->currentRow() == 1) {
        QMessageBox msgbox(QMessageBox::Critical, "清空", "你确定要清空【已经下载】列表吗？", QMessageBox::Ok | QMessageBox::Cancel);
        if (msgbox.exec() == QMessageBox::Ok) {
            ui->listWidgetDownloaded->clear();
            saveList("downloaded");
        }
    } else if (ui->listWidgetNav->currentRow() == 2) {
        ui->listWidgetTrash->clear();
        saveList("trash");
    }
}

void MainWindow::on_actionDirectory_triggered()
{
    if (ui->listWidgetNav->currentRow() == 0) {
        if (ui->listWidgetDownloading->count() != 0) {
            QString path = ((Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(ui->listWidgetDownloading->currentRow()))))->ui->labelPath->text();
            qDebug() << path;
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        }
    } else if (ui->listWidgetNav->currentRow() == 1) {
        if (ui->listWidgetDownloading->count() != 0) {
            QString path = ((Form*)(ui->listWidgetDownloaded->itemWidget(ui->listWidgetDownloaded->item(ui->listWidgetDownloaded->currentRow()))))->ui->labelPath->text();
            qDebug() << path;
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        }
    } else if (ui->listWidgetNav->currentRow() == 2) {
        if(ui->listWidgetDownloading->count() != 0){
            QString path = ((Form*)(ui->listWidgetTrash->itemWidget(ui->listWidgetTrash->item(ui->listWidgetTrash->currentRow()))))->ui->labelPath->text();
            qDebug() << path;
            QDesktopServices::openUrl(QUrl::fromLocalFile(path));
        }
    }
}

void MainWindow::addNew()
{
    QDateTime time = QDateTime::currentDateTime();
    QString stime = time.toString("yyyy-MM-dd hh:mm:ss");
    QString sfn = dialogNew->ui->lineEditFilename->text();
    QString spath = dialogNew->ui->lineEditPath->text();
    QString surl = dialogNew->ui->textEditURL->toPlainText();
    if (surl.startsWith("thunder://")) {
        qDebug() << "thunder://" << surl.mid(10);
        QByteArray BA = QByteArray::fromBase64(surl.mid(10).toUtf8()).mid(2);
        surl = QByteArray::fromPercentEncoding(BA.mid(0,BA.length()-2));
        qDebug() << "迅雷协议解码：" << surl;
        sfn = QFileInfo(surl).fileName();
        sfn = sfn.left(sfn.indexOf("?"));
        Form *form = new Form;
        form->ui->labelFilename->setText(sfn);
        form->ui->labelURL->setText(surl);
        form->ui->labelURL->adjustSize();
        form->ui->labelPath->setText(spath);
        form->ui->labelTimeCreate->setText(stime);
        QListWidgetItem *LWI = new QListWidgetItem(ui->listWidgetDownloading);
        ui->listWidgetDownloading->setItemWidget(LWI,form);
        LWI->setSizeHint(QSize(1200,30));
        ui->listWidgetDownloading->addItem(LWI);
        form->download(surl);
        connect(form,SIGNAL(downloadFinish()),this,SLOT(moveToDownloaded()));
        saveList("downloading");
    } else {
        QStringList SL = surl.split("\n");
        for(int i=0; i<SL.size(); i++){
            Form *form = new Form;
            form->ui->labelFilename->setText(QFileInfo(SL.at(i)).fileName());
            form->ui->labelURL->setText(SL.at(i));
            form->ui->labelURL->adjustSize();
            form->ui->labelPath->setText(spath);
            form->ui->labelTimeCreate->setText(stime);
            QListWidgetItem *LWI = new QListWidgetItem(ui->listWidgetDownloading);
            ui->listWidgetDownloading->setItemWidget(LWI,form);
            LWI->setSizeHint(QSize(1200,30));
            ui->listWidgetDownloading->addItem(LWI);
            form->download(SL.at(i));
            connect(form,SIGNAL(downloadFinish()),this,SLOT(moveToDownloaded()));
            saveList("downloading");
        }
    }
}

void MainWindow::itemClick(QListWidgetItem *item)
{
    Q_UNUSED(item);
    //qDebug() << item->text();
    switch (ui->listWidgetNav->currentRow()) {
    case 0:
        ui->listWidgetDownloading->show();
        ui->listWidgetDownloaded->hide();
        ui->listWidgetTrash->hide();
        break;
    case 1:
        ui->listWidgetDownloading->hide();
        loadList("downloaded");
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

//接收不到信号
void MainWindow::moveToDownloaded()
{
    qDebug() << "moveToDownloaded";
    Form *form = qobject_cast<Form*>(sender());
    QListWidgetItem *LWI = new QListWidgetItem(ui->listWidgetDownloaded);
    ui->listWidgetDownloaded->addItem(LWI);
    ui->listWidgetDownloaded->setItemWidget(LWI,form);
    LWI->setSizeHint(QSize(1200,30));
    for (int i=0; i<ui->listWidgetDownloading->count(); i++) {
        if (form == ((Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(i))))) {
            ui->listWidgetDownloading->takeItem(i);
        }
    }
}

void MainWindow::loadList(QString filename)
{
    if (filename == "downloading") {
        ui->listWidgetDownloading->clear();
    }else if(filename == "downloaded") {
        ui->listWidgetDownloaded->clear();
    }else if(filename == "trash") {
        ui->listWidgetTrash->clear();
    }
    QString filepath = QDir::currentPath() + "/" + filename;
    QFile *file = new QFile(filepath);
    if (file->open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream ts(file);
        QString s = ts.readAll();
        file->close();
        if (s != "") {
            QStringList line = s.split("\n");
            for (int i=0; i<line.size(); i++) {
                Form *form = new Form;
                if (line.at(i) != "" ) {
                    if (line.at(i).contains("#")) {
                        QStringList strlist = line.at(i).split("#");
                        form->ui->labelFilename->setText(strlist.at(0));
                        form->ui->labelSize->setText(strlist.at(1));
                        form->ui->progressBar->setRange(0,strlist.at(3).toInt());
                        form->ui->progressBar->setValue(strlist.at(2).toInt());
                        form->ui->labelSpeed->setText(strlist.at(4));
                        form->ui->labelURL->setText(strlist.at(5));
                        form->ui->labelURL->adjustSize();
                        form->ui->labelElapse->setText(strlist.at(6));
                        form->ui->labelTimeCreate->setText(strlist.at(7));
                        form->ui->labelPath->setText(strlist.at(8));
                    }
                    if (filename == "downloading") {
                        QListWidgetItem *LWI = new QListWidgetItem(ui->listWidgetDownloading);
                        ui->listWidgetDownloading->setItemWidget(LWI,form);
                        LWI->setSizeHint(QSize(1200,30));
                        ui->listWidgetDownloading->addItem(LWI);
                    } else if (filename == "downloaded") {
                        QListWidgetItem *LWI = new QListWidgetItem(ui->listWidgetDownloaded);
                        ui->listWidgetDownloaded->setItemWidget(LWI,form);
                        LWI->setSizeHint(QSize(1200,30));
                        ui->listWidgetDownloaded->addItem(LWI);
                    } else if (filename == "trash") {
                        QListWidgetItem *LWI = new QListWidgetItem(ui->listWidgetTrash);
                        ui->listWidgetTrash->setItemWidget(LWI,form);
                        LWI->setSizeHint(QSize(1200,30));
                        ui->listWidgetTrash->addItem(LWI);
                    }
                }
            }
        }
    }
}

void MainWindow::appendList(QString filenameWrite, QString filename, QString size, int progressmax, int progress, QString speed, QString url, QString elapse, QString timeCreate, QString path)
{
    QString filepath = QDir::currentPath() + "/" + filenameWrite;
    QFile file(filepath);
    if(file.open(QFile::WriteOnly | QIODevice::Append)){
        QTextStream ts(&file);
        QString s = filename + "#" + size + "#" + QString::number(progressmax) + "#" + QString::number(progress) + "#" + speed + "#" + url + "#" + elapse + "#" + timeCreate + "#" + path + "\n";
        ts << s;
        file.close();
    }
}

void MainWindow::saveList(QString filename)
{
    QString filepath = QDir::currentPath() + "/" + filename;
    QFile file(filepath);
    if(file.open(QFile::WriteOnly)){
        QTextStream ts(&file);
        QString s="";
        if(filename == "downloading"){
            for(int i=0; i<ui->listWidgetDownloading->count(); i++){
                Form *form = (Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(i)));
                s = s + form->ui->labelFilename->text() + "#" + form->ui->labelSize->text() + "#" + QString::number(form->ui->progressBar->maximum()) + "#" + QString::number(form->ui->progressBar->value()) + "#" + form->ui->labelSpeed->text() + "#" + form->ui->labelURL->text() + "#" + form->ui->labelElapse->text() + "#" + form->ui->labelTimeCreate->text() + "#" + form->ui->labelPath->text();
                if(i < ui->listWidgetDownloading->count()-1)
                    s = s + "\n";
            }
        }else if(filename == "downloaded"){
            for(int i=0; i<ui->listWidgetDownloaded->count(); i++){
                Form *form = (Form*)(ui->listWidgetDownloaded->itemWidget(ui->listWidgetDownloaded->item(i)));
                s = s + form->ui->labelFilename->text() + "#" + form->ui->labelSize->text() + "#" + QString::number(form->ui->progressBar->maximum()) + "#" + QString::number(form->ui->progressBar->value()) + "#" + form->ui->labelSpeed->text() + "#" + form->ui->labelURL->text() + "#" + form->ui->labelElapse->text() + "#" + form->ui->labelTimeCreate->text() + "#" + form->ui->labelPath->text();
                if(i < ui->listWidgetDownloaded->count()-1)
                    s = s + "\n";
            }
        }else if(filename == "trash"){
            for(int i=0; i<ui->listWidgetTrash->count(); i++){
                Form *form = (Form*)(ui->listWidgetTrash->itemWidget(ui->listWidgetTrash->item(i)));
                s = s + form->ui->labelFilename->text() + "#" + form->ui->labelSize->text() + "#" + QString::number(form->ui->progressBar->value()) + "#" + QString::number(form->ui->progressBar->maximum()) + "#" + form->ui->labelSpeed->text() + "#" + form->ui->labelURL->text() + "#" + form->ui->labelElapse->text() + "#" + form->ui->labelTimeCreate->text() + "#" + form->ui->labelPath->text();
                if(i < ui->listWidgetTrash->count()-1)
                    s = s + "\n";
            }
        }
        ts << s;
        file.close();
    }
}

void MainWindow::viewContextMenu(const QPoint &position)
{
    QAction *action_copyurl;
    QAction *result_action;
    QModelIndex index = ui->listWidgetDownloading->indexAt(position);
    QList<QAction *> actions;
    action_copyurl = new QAction(this);
    action_copyurl->setText("复制下载地址");
    actions.append(action_copyurl);
    if(index.isValid()){
        result_action = QMenu::exec(actions,ui->listWidgetDownloading->mapToGlobal(position));
    }
    if(result_action == action_copyurl) {
        Form *form = (Form*)(ui->listWidgetDownloading->itemWidget(ui->listWidgetDownloading->item(ui->listWidgetDownloading->currentRow())));
        QString s = form->ui->labelURL->text();
        QClipboard *clipboard = QApplication::clipboard();
        clipboard->setText(s);
        return;
    }

}

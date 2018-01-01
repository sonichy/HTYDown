#include "dialognew.h"
#include "ui_dialognew.h"
#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

DialogNew::DialogNew(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNew)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    connect(ui->btnBrowse, SIGNAL(clicked()), this, SLOT(browse()));
    connect(ui->textEditURL,SIGNAL(textChanged()),this,SLOT(textChange()));
    ui->lineEditPath->setText(QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).first());
}

DialogNew::~DialogNew()
{
    delete ui;
}

void DialogNew::browse()
{
    dir = QFileDialog::getExistingDirectory(this,"保存路径",path, QFileDialog::ShowDirsOnly |QFileDialog::DontResolveSymlinks);
    if(dir!=""){
        path=dir;
        ui->lineEditPath->setText(path);
    }
}

void DialogNew::textChange()
{
    QString s = ui->textEditURL->toPlainText();
    if(s.startsWith("thunder://")){
        QByteArray BA = QByteArray::fromBase64(s.mid(10).toUtf8()).mid(2);
        QString surl = QByteArray::fromPercentEncoding(BA.mid(0,BA.length()-2));
        ui->lineEditDecode->setText(surl);
        ui->lineEditDecode->setCursorPosition(0);
        QString fn = QFileInfo(surl).fileName();
        fn = fn.left(fn.indexOf("?"));
        ui->lineEditFilename->setText(fn);
    }else{
        QString LEFN ="";
        QStringList SL = s.split("\n");
        for(int i=0; i<SL.size(); i++){
            QString fn = QFileInfo(SL.at(i)).fileName();
            fn = fn.left(fn.indexOf("?"));
            LEFN += fn + "; ";
        }
        ui->lineEditFilename->setText(LEFN);
    }
}

void DialogNew::checkWriteable()
{
    QString filepath = ui->lineEditPath->text() + "/temp";
    QFile *file = new QFile(filepath);
    bool b = file->open(QIODevice::WriteOnly);
    if(b){
        file->close();
        file->remove();
    }else{
        QMessageBox::critical(NULL, "错误", "目录无法写入！");
    }
}

void DialogNew::accept()
{
    QString spath = ui->lineEditPath->text();
    QDir dir(spath);
    if(dir.exists()){
        checkWriteable();
    }else{
        bool ok = dir.mkdir(spath);
        if(ok){
            QDialog::accept();
        }else{
            QMessageBox::critical(NULL, "错误", "无法创建文件夹！");
        }
    }

}

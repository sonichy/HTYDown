#include "dialognew.h"
#include "ui_dialognew.h"
#include <QFileDialog>
#include <QStandardPaths>

DialogNew::DialogNew(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNew)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    connect(ui->btnBrowse, SIGNAL(clicked()), this, SLOT(browse()));
    connect(ui->lineEditURL,SIGNAL(textChanged(QString)),this,SLOT(textChange(QString)));
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

void DialogNew::textChange(QString s)
{
    if(s.startsWith("thunder://")){
        QByteArray BA = QByteArray::fromBase64(s.mid(10).toUtf8()).mid(2);
        QString surl = QByteArray::fromPercentEncoding(BA.mid(0,BA.length()-2));
        ui->lineEditDecode->setText(surl);
        ui->lineEditDecode->setCursorPosition(0);
        QString fn = QFileInfo(surl).fileName();
        fn = fn.left(fn.indexOf("?"));
        ui->lineEditFilename->setText(fn);
    }else{
        QString fn = QFileInfo(s).fileName();
        fn = fn.left(fn.indexOf("?"));
        ui->lineEditFilename->setText(fn);
    }
}

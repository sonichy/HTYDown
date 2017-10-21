#include "form.h"
#include "ui_form.h"
#include <QNetworkAccessManager>
#include <QFile>
#include <QDebug>

Form::Form(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Form)
{
    ui->setupUi(this);    
}

Form::~Form()
{
    delete ui;
}

void Form::download(QString surl)
{
    timer.start();
    QUrl url(surl);
    QNetworkAccessManager manager;
    QEventLoop loop;
    reply = manager.get(QNetworkRequest(url));    
    connect(reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(updateProgress(qint64,qint64)));
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    QString filename = ui->labelPath->text() + "/" + ui->labelFilename->text();
    qDebug() << filename;
    QFile file(filename);
    file.open(QIODevice::WriteOnly);
    file.write(reply->readAll());
    file.close();
    emit downloadFinish();
}

QString sbytes(qint64 bytes){
    QString unit = "B";
    double dbytes = bytes*1.0;
    if(bytes > 999999999){
        dbytes /= (1024*1024*1024);
        unit="GB";
    }else{
        if(bytes > 999999){
            dbytes /= (1024*1024);
            unit="MB";
        }else{
            if(bytes > 999){
                dbytes /= 1024;
                unit = "KB";
            }
        }
    }
    return QString("%1%2").arg(QString::number(dbytes,'f',2)).arg(unit);
}

QString sspeed(int speed){
    QString unit = "B/s";
    double dspeed = speed*1.0;
    if(speed > 999999999){
        dspeed /= (1024*1024*1024);
        unit = "GB/s";
    }else{
        if(speed > 999999){
            dspeed /= (1024*1024);
            unit = "MB/s";
        }else{
            if(speed > 999){
                dspeed /= 1024;
                unit = "KB/s";
            }
        }
    }
    return QString("%1%2").arg(QString::number(dspeed,'f',2)).arg(unit);
}

void Form::updateProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    int elapse = timer.elapsed();
    QTime t(0,0,0);
    t = t.addMSecs(elapse);
    QString selapse = t.toString("hh:mm:ss");
    int speed = bytesReceived*1000/elapse;
    ui->labelSize->setText(QString("%1 / %2").arg(sbytes(bytesReceived)).arg(sbytes(bytesTotal)));
    //qDebug() << bytesTotal << "/2000000000=" << bytesTotal/2000000000;
    if(bytesTotal < 2000000000){
        ui->progressBar->setMaximum(bytesTotal);
        ui->progressBar->setValue(bytesReceived);
    }else{
        ui->progressBar->setMaximum(bytesTotal/2000000000);
        ui->progressBar->setValue(bytesReceived/2000000000);
    }
    ui->labelSpeed->setText(QString("%1").arg(sspeed(speed)));
    ui->labelElapse->setText(selapse);
}

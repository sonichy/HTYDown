#ifndef FORM_H
#define FORM_H

#include <QWidget>
#include <QElapsedTimer>
#include <QNetworkReply>

namespace Ui {
class Form;
}

class Form : public QWidget
{
    Q_OBJECT

public:
    explicit Form(QWidget *parent = 0);
    ~Form();
    Ui::Form *ui;
    QNetworkReply *reply;
    QElapsedTimer timer;
    void download(QString url);

private slots:
    void updateProgress(qint64,qint64);
};

#endif // FORM_H

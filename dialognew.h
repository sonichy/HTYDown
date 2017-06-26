#ifndef DIALOGNEW_H
#define DIALOGNEW_H

#include <QDialog>

namespace Ui {
class DialogNew;
}

class DialogNew : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNew(QWidget *parent = 0);
    ~DialogNew();
    Ui::DialogNew *ui;

private:
    QString dir,path;

public slots:
    void getFilename(QString);

private slots:
    void browse();
};

#endif // DIALOGNEW_H

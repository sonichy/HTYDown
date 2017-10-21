#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include "dialognew.h"
#include "ui_dialognew.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    DialogNew *dialognew;

private slots:
    void on_action_quit_triggered();
    void on_action_changelog_triggered();
    void on_action_aboutQt_triggered();
    void on_action_about_triggered();
    void on_actionStart_triggered();
    void on_actionPause_triggered();
    void on_actionDelete_triggered();
    void on_actionDirectory_triggered();
    void showDialogNew();
    void addnew();
    void itemClick(QListWidgetItem *item);
    void moveToDownloaded();
};

#endif // MAINWINDOW_H

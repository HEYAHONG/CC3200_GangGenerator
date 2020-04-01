#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QMessageBox>
#include <unistd.h>
#include <QFile>
#include <QDebug>
#include <QPlainTextEdit>
#include <QScrollBar>
#include <QFileDialog>
#include "setflashsize.h"

#ifdef WIN32

#include "windows.h"
#include "winuser.h"
#include "stdio.h"
#endif


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_log_textChanged();

    void on_action_triggered();

    void on_actionFlash_triggered();

    void on_action_5_triggered();

    void on_action_2_triggered();

    void on_action_3_triggered();

    void on_outputdir_inputRejected();

    void on_outputdir_selectionChanged();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_action_mcuimg_bin_triggered();

private:
    Ui::MainWindow *ui;

    SetFlashSize *flashsize;

    //准备好文件，将qrc中的文件释放到临时文件夹中
    QString rc_dir;
    QString Execute_Path;
    void log(QString data);
    void prepare_files();

    //列出目录中的文件
    std::vector<QString> list_dir(QString dir);
};
#endif // MAINWINDOW_H

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
#include <QDomComment>
#include <QMutex>
#include <QQueue>
#include <QTimer>
#include "setflashsize.h"
#include <buildthread.h>
#include <cmdsettings.h>

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

    void on_action_GangImage_triggered();

    void on_action_6_triggered();

private:
    Ui::MainWindow *ui;

    SetFlashSize *flashsize;

    //准备好文件，将qrc中的文件释放到临时文件夹中
    QString rc_dir;
    QString Execute_Path;

    void prepare_files();

    //列出目录中的文件
    std::vector<QString> list_dir(QString dir);
    //config.xml生成和文件输出
    void ConfigXmlGenAndOutput();

    //日志窗口相关
    QQueue<QString> log_queue;
    QTimer          *log_timer;
    QMutex          *log_mutex;

    //命令执行窗口
    cmdsettings *cmd;

public:
    void log(QString data);
public slots:
    void log_timer_timeout();
    void BuildThread_fininshed(bool Is_Normal);
};
#endif // MAINWINDOW_H

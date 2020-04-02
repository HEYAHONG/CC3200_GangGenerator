#ifndef BUILDTHREAD_H
#define BUILDTHREAD_H
#include <QThread>
#include <mainwindow.h>
#ifdef WIN32
#include "windows.h"
#else
#error "暂时只支持windows"
#endif

class BuildThread : public QThread
{
public:
    BuildThread(QString _cmd,void *_main);
    ~BuildThread();
    QString cmd;
    void *main;
    void run() override;
#ifdef WIN32
    HANDLE hChildStd_IN_Rd ;
    HANDLE hChildStd_IN_Wr ;
    HANDLE hChildStd_OUT_Rd ;
    HANDLE hChildStd_OUT_Wr ;
    PROCESS_INFORMATION piProcInfo;
    SECURITY_ATTRIBUTES saAttr;
#endif
    void log(QString data);
};

#endif // BUILDTHREAD_H

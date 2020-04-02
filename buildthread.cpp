#include "buildthread.h"

BuildThread::BuildThread(QString _cmd,void * _main)
{
cmd=_cmd;
main=_main;
#ifdef WIN32
hChildStd_IN_Rd=NULL;
hChildStd_IN_Wr=NULL;
hChildStd_OUT_Rd=NULL;
hChildStd_OUT_Wr=NULL;
piProcInfo.hThread=NULL;
piProcInfo.hProcess=NULL;
#endif
start();
}

BuildThread::~BuildThread()
{
#ifdef WIN32
    if(hChildStd_IN_Rd!=NULL)
        CloseHandle(hChildStd_IN_Rd);
    if(hChildStd_IN_Wr!=NULL)
        CloseHandle(hChildStd_IN_Wr);
    if(hChildStd_OUT_Rd!=NULL)
        CloseHandle(hChildStd_OUT_Rd);
    if(hChildStd_OUT_Wr!=NULL)
        CloseHandle(hChildStd_OUT_Wr);
    if(piProcInfo.hThread!=NULL)
        CloseHandle(piProcInfo.hThread);
    if(piProcInfo.hProcess!=NULL)
        CloseHandle(piProcInfo.hProcess);
#endif
}

class ReadThread: public QThread
{
public:
    BuildThread *parent;
    bool Is_Run;
    ReadThread(BuildThread *t)
    {
        parent=t;
        Is_Run=true;
        start();
    }
    void run() override
    {
        while(Is_Run)
        {
            char buff[256]={0};
#ifdef WIN32
            DWORD dwRead=0;
            ReadFile(parent->hChildStd_OUT_Rd,buff,sizeof(buff),&dwRead,NULL);
            if(dwRead!=0)
            {
                parent->log(buff);
            }
#endif

        }
    }

};

void BuildThread::run()
{
#ifdef WIN32

ZeroMemory( &piProcInfo, sizeof(PROCESS_INFORMATION) );
ZeroMemory(&saAttr,sizeof (saAttr));
saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
saAttr.bInheritHandle = TRUE;
saAttr.lpSecurityDescriptor = NULL;

// Create a pipe for the child process's STDOUT.

if ( ! CreatePipe(&hChildStd_OUT_Rd, &hChildStd_OUT_Wr, &saAttr, 0) )
      {
            return;
      }

// Ensure the read handle to the pipe for STDOUT is not inherited.

if ( ! SetHandleInformation(hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0) )
      {
        return;
      }

// Create a pipe for the child process's STDIN.

if (! CreatePipe(&hChildStd_IN_Rd, &hChildStd_IN_Wr, &saAttr, 0))
      {
          return ;
      }

// Ensure the write handle to the pipe for STDIN is not inherited.

if ( ! SetHandleInformation(hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0) )
      {

          return;
      }

{
    STARTUPINFOA siStartInfo;
    ZeroMemory( &siStartInfo, sizeof(STARTUPINFO) );
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = hChildStd_OUT_Wr;
    siStartInfo.hStdOutput = hChildStd_OUT_Wr;
    siStartInfo.hStdInput = hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;
    siStartInfo.dwFlags |= STARTF_USESHOWWINDOW;
    siStartInfo.wShowWindow=SW_HIDE;
    BOOL IsOK=CreateProcessA(
          NULL,
          (LPSTR)cmd.toStdString().c_str(),     // command line
          NULL,          // process security attributes
          NULL,          // primary thread security attributes
          TRUE,          // handles are inherited
          0,             // creation flags
          NULL,          // use parent's environment
          NULL,          // use parent's current directory
          &siStartInfo,  // STARTUPINFO pointer
          &piProcInfo);  // receives PROCESS_INFORMATION
    if(IsOK)
    {
       ReadThread *t=new ReadThread(this);
       while(1)
       {
        if(WAIT_OBJECT_0==WaitForSingleObject(piProcInfo.hProcess,5))

        {
            break;//程序结束
        }
       }
       t->terminate();
    }

}

#endif
}

void BuildThread::log(QString data)
{
    ((MainWindow *)main)->log(data);
}

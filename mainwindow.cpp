#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //设置窗口固定大小
    setFixedSize(size());
    //设置图标
    setWindowIcon(QIcon(":/qrc/share/icon/window_resources.ico"));
    //设置窗口标题
    setWindowTitle("CC3200_GangGenerator");

    //初始化logtimer
    log_timer=new QTimer(this);
    connect(log_timer,SIGNAL(timeout()),this,SLOT(log_timer_timeout()));
    log_timer->start(5);
    log_mutex =new QMutex(QMutex::Recursive);

    //初始化脚本窗口
    cmd=new cmdsettings(this);

    //显示帮助
    {
        QFile help;
        help.setFileName(":/qrc/share/help.txt");
        help.open(QIODevice::ReadOnly);
        log(help.readAll());
    }


    //准备好所需文件
    prepare_files();

    //设置好输入和输出的文件夹路径
    ui->inputdir->setText(rc_dir+"/input/");
    ui->outputdir->setText(rc_dir+"/output/");

    //准备好窗口控件
    flashsize=new SetFlashSize(this);




}


//删除文件夹
static bool deleteDir(const QString path)
{
    if (path.isEmpty()){
        return false;
    }
    QDir dir(path);
    if(!dir.exists()){
        return true;
    }
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
    QFileInfoList fileList = dir.entryInfoList(); // 获取所有的文件信息
    foreach (QFileInfo file, fileList){ //遍历文件信息
        if (file.isFile()){ // 是文件，删除
            file.dir().remove(file.fileName());
        }else{ // 递归删除
            deleteDir(file.absoluteFilePath());
        }
    }
    return dir.rmpath(dir.absolutePath()); // 删除文件夹
}


void MainWindow::prepare_files()
{
    //要提取的文件列表
    const std::vector<QString> file_list=
    {
        "/bin/linux/BuildGangImage.bin",
        "/bin/linux/bz2.i386-linux-gnu.so",
        "/bin/linux/Crypto.Cipher._AES.so",
        "/bin/linux/Crypto.Hash._SHA256.so",
        "/bin/linux/Crypto.PublicKey._fastmath.so",
        "/bin/linux/Crypto.Util._counter.so",
        "/bin/linux/datetime.i386-linux-gnu.so",
        "/bin/linux/GangProgramSec.bin",
        "/bin/linux/libFlashAPILinux.so",
        "/bin/linux/libpython2.7.so.1.0",
        "/bin/linux/library.zip",
        "/bin/linux/mmap.i386-linux-gnu.so",
        "/bin/linux/pyexpat.i386-linux-gnu.so",
        "/bin/linux/readline.i386-linux-gnu.so",
        "/bin/linux/termios.i386-linux-gnu.so",
        "/bin/linux/_codecs_cn.i386-linux-gnu.so",
        "/bin/linux/_codecs_hk.i386-linux-gnu.so",
        "/bin/linux/_codecs_iso2022.i386-linux-gnu.so",
        "/bin/linux/_codecs_jp.i386-linux-gnu.so",
        "/bin/linux/_codecs_kr.i386-linux-gnu.so",
        "/bin/linux/_codecs_tw.i386-linux-gnu.so",
        "/bin/linux/_ctypes.i386-linux-gnu.so",
        "/bin/linux/_hashlib.i386-linux-gnu.so",
        "/bin/linux/_multibytecodec.i386-linux-gnu.so",
        "/bin/linux/_ssl.i386-linux-gnu.so",
        "/bin/rbtl3100.dll",
        "/bin/rbtl3100s.dll",
        "/bin/rbtl3101.dll",
        "/bin/rbtl3101_121.dll",
        "/bin/rbtl3101_132.dll",
        "/bin/win/BuildGangImage.exe",
        "/bin/win/FlashAPI.dll"
    };

    {//获得临时文件夹路径,执行文件所在目录
#ifdef WIN32
        char temp[_MAX_PATH]={0};
        size_t length=0;
        length=GetTempPathA(sizeof(temp),temp);
        for(size_t i=0;i<length;i++)
        {
            if(temp[i]=='\\')
                temp[i]='/';
        }
        if(strlen(temp)==0 || access(temp,0)<0)
        {
            rc_dir="./";
        }
        else
        {
            //获得长文件名
            GetLongPathNameA(temp,temp,sizeof(temp));
            //设置释放文件夹为随机文件夹
            srand(time(NULL));
            rc_dir=(QString)temp+"/CC3200_GangGenerator"+QString::number(rand())+"/";
        }
        ui->statusbar->showMessage(QString("临时文件夹为:")+rc_dir);
        Execute_Path=rc_dir+"/bin/win/";
#else
#error "暂时只支持windows"
#endif
    }

    {//准备文件夹
        QDir dir;
        dir.setCurrent(rc_dir);
        dir.mkpath(rc_dir+"/input");
        dir.mkpath(rc_dir+"/output");
    }

    {//释放文件

        log("准备所需文件...\n");
        for(size_t i=0;i<file_list.size();i++)
        {
            QFile file,file1;
            file.setFileName((QString)":/qrc/"+file_list[i]);
            file1.setFileName(rc_dir+"/"+file_list[i]);
            if(file.open(QIODevice::ReadOnly))
            {

                ui->statusbar->showMessage((QString)"正在释放："+file_list[i]);
                QByteArray filedata=file.readAll();
                {
                    QFileInfo info(file1);
                    QDir dir;
                    dir.setCurrent(rc_dir);
                    dir.mkpath(info.dir().path());
                }
                if(!file1.open(QIODevice::ReadWrite|QIODevice::NewOnly))
                {
                    ui->statusbar->showMessage(file_list[i]+":创建临时目录失败！");
                }

                if(file1.write(filedata))
                {
                   ui->statusbar->showMessage((QString)"释放"+file_list[i]+"成功");
                }
                else
                {
                   QMessageBox::warning(this,"警告",file_list[i]+"释放失败");
                }
            }
        }
        ui->statusbar->showMessage("释放文件操作完成!",3000);


    }

    {//设置path变量
        QString Path=qgetenv("PATH");
#ifndef WIN32
        Path=Execute_Path+":"+Path;
#else
        Path=Execute_Path+";"+Path;
#endif
        qputenv("PATH",QByteArray(Path.toStdString().c_str(),Path.length()));
    }

    {//测试运行环境设置是否正确

    }

}

MainWindow::~MainWindow()
{
    {//删除使用的文件夹
        deleteDir(rc_dir);
    }
    delete ui;
}


void MainWindow::on_log_textChanged()
{//
    ui->log->textCursor().movePosition(QTextCursor::End);
    ui->log->verticalScrollBar()->setSliderPosition(ui->log->verticalScrollBar()->maximum());
    ui->log->moveCursor(QTextCursor::End);

}
void MainWindow::log(QString data)
{
    log_mutex->lock();
    log_queue.push_back(data);
    log_mutex->unlock();
}

void MainWindow::on_action_triggered()
{
    QMessageBox::information(this,"提示","CC3200 GangImage生成器 BY HYH");
}

void MainWindow::on_actionFlash_triggered()
{//芯片容量设置窗口
    flashsize->show();
}

void MainWindow::on_action_5_triggered()
{//退出窗口
    close();
}

void MainWindow::on_action_2_triggered()
{//设置导入的文件夹
    QString path=QFileDialog::getExistingDirectory(this,"设置导入的文件夹：","");
    if(path.length()==0)
    {
        ui->statusbar->showMessage("操作取消\n");
        return;
    }
    ui->inputdir->setText(path);
}

void MainWindow::on_action_3_triggered()
{//设置输出的文件夹
    QString path=QFileDialog::getExistingDirectory(this,"设置输出的文件夹：","");
    if(path.length()==0)
    {
        ui->statusbar->showMessage("操作取消\n");
        return;
    }
    ui->outputdir->setText(path);
    {
        QDir dir_input,dir_output;
        dir_input.setPath(ui->inputdir->text());
        dir_output.setPath(ui->outputdir->text());
        if(dir_input.path() == dir_output.path())
        {
            QMessageBox::warning(this,"警告","输出的文件夹不能与导入文件夹相同，请新建一个输出文件夹!");
            on_action_3_triggered();//重新选择文件夹
        }
    }
}

void MainWindow::on_outputdir_inputRejected()
{//

}

void MainWindow::on_outputdir_selectionChanged()
{

}

void MainWindow::on_pushButton_clicked()
{//打开输入文件夹
QString path=ui->inputdir->text();
path.replace("//","/");
path.replace('/','\\');
log("-------------------------------------------------\n");
log("输入文件目录中包含的文件：\n");
foreach(QString filepath,list_dir(ui->inputdir->text()))
{
    log(filepath+"\n");
}
log("-------------------------------------------------\n");
#ifdef WIN32
    WinExec((QString("explorer ")+path).toStdString().c_str(),SW_SHOW);
#else
#error "暂时只支持windows"
#endif
}

void MainWindow::on_pushButton_2_clicked()
{//打开输出文件夹
    QString path=ui->outputdir->text();
    path.replace("//","/");
    path.replace('/','\\');
#ifdef WIN32
    WinExec((QString("explorer ")+path).toStdString().c_str(),SW_SHOW);
#else
#error "暂时只支持windows"
#endif
}

void MainWindow::on_action_mcuimg_bin_triggered()
{//读入mcuimg
    QString mcuimg_path=QFileDialog::getOpenFileName(this,"打开mcuimg.bin","","bin文件 (*.bin)");
    if(mcuimg_path.length()==0)
    {
        ui->statusbar->showMessage("操作取消！\n");
        return;
    }
    QFile file;
    file.setFileName(ui->inputdir->text()+"/sys/mcuimg.bin");
    {
        QDir dir;
        dir.mkpath(QFileInfo(file).dir().path());
    }
    if(!file.copy(mcuimg_path,ui->inputdir->text()+"/sys/mcuimg.bin"))
    {
        QMessageBox::warning(this,"警告","复制文件失败!\n");
    };

}

std::vector<QString> MainWindow::list_dir(QString dir)
{
   std::vector<QString> file_list;
   QDir current_dir;
   current_dir.setPath(dir);
   if(!current_dir.exists())
   {
       return file_list;
   }
   current_dir.currentPath();
   {//循环遍历
       current_dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot); //设置过滤
       QFileInfoList list=current_dir.entryInfoList();
       foreach(QFileInfo file_info,list)
       {
           if(file_info.isFile())
           {
               file_list.push_back(QString("/")+file_info.fileName());
           }
           else
           {//遍历文件夹
               std::vector<QString> next_list=list_dir(file_info.absoluteFilePath());
               current_dir.currentPath();
               foreach(QString filepath,next_list)
               {
                   file_list.push_back(QString("/")+file_info.fileName()+filepath);
               }

           }
       }
   }

   return file_list;
}

 void MainWindow:: ConfigXmlGenAndOutput()
 {
     //定义输出的目录路径
     QString outpath=ui->outputdir->text()+"/GangImageConfig.xml";
     outpath.replace("//","/");

     {//设置环境变量
#ifndef WIN32
         qputenv("INPUTDIR",ui->inputdir->text().replace("//","/").toStdString().c_str());
         qputenv("OUTPUTDIR",ui->outputdir->text().replace("//","/").toStdString().c_str());
#else
         qputenv("INPUTDIR",ui->inputdir->text().replace("//","/").replace("/","\\").toStdString().c_str());
         qputenv("OUTPUTDIR",ui->outputdir->text().replace("//","/").replace("/","\\").toStdString().c_str());
#endif
     }

     {//执行脚本
         foreach(cmd_struct cmd,cmd->getData())
         {
             if(cmd.flags & CMD_FLAGS_BEFORE_LIST)
             {
                 if(cmd.cmd.length()>0)
                 {
                     system(cmd.cmd.toStdString().c_str());
                 }
             }
         }
     }

    //定义Xml文件变量
     QDomDocument xml;
     log("-------------------------------------------------\n");
     log("准备GangImageConfig.xml\n\r");
     {//加载模板,模板中节点的顺序是固定的
         QFile file;
         file.setFileName(":/qrc/share/GangImageConfig.xml");
         file.open(QIODevice::ReadOnly);
         xml.setContent(file.readAll());
         file.close();
     }
     QDomElement root=xml.documentElement();
     QDomNode node=root.firstChild(); //获得第一个子节点

     {//设置flashsize
         log("设置flash大小\n\r");
         while(!node.isElement()) node=node.nextSibling();
         node.toElement().setAttribute("StorageCapacityBytes",QString::number(flashsize->flash_size*1024));
     }

     {//移动到GangCommandsSet节点
         node=node.nextSibling();
         while(!node.isElement()) node=node.nextSibling();
         node=node.firstChild();
         while(!node.isElement()) node=node.nextSibling();
     }

     {//添加格式化命令
         log("添加格式化命令\n");
         QDomElement cmd=xml.createElement("Command");

         QDomElement CommandFormatStorage=xml.createElement("CommandFormatStorage");

         QDomElement ReservedBlocksBitmap=xml.createElement("ReservedBlocksBitmap");
         ReservedBlocksBitmap.appendChild(xml.createTextNode("402653184"));

         QDomElement FormatStorageFlagsList=xml.createElement("FormatStorageFlagsList");

         CommandFormatStorage.appendChild(ReservedBlocksBitmap);
         CommandFormatStorage.appendChild(FormatStorageFlagsList);

         cmd.appendChild(CommandFormatStorage);

         //添加到GangCommandsSet节点
         node.appendChild(cmd);

     }

     {//添加写文件命令
         log("添加写文件命令\n");
         foreach(QString filename,list_dir(ui->inputdir->text()))
         {
            log(filename+"\n");

            QDomElement cmd=xml.createElement("Command");

            QDomElement CommandWriteFile=xml.createElement("CommandWriteFile");
            CommandWriteFile.setAttribute("CertificationFileName","");
            CommandWriteFile.setAttribute("SignatureFileName","");

            QDomElement FileSystemName=xml.createElement("FileSystemName");
            FileSystemName.appendChild(xml.createTextNode(filename));

            QDomElement FileOpenFlagsList=xml.createElement("FileOpenFlagsList");
            QDomElement FileOpenFlag=xml.createElement("FileOpenFlag");
            FileOpenFlag.appendChild(xml.createTextNode("FILE_OPEN_FLAG_FAILSAFE"));
            FileOpenFlagsList.appendChild(FileOpenFlag);


                QString path=ui->inputdir->text()+filename;
                path.replace("//","/");
                QFile file;
                file.setFileName(path);
                file.open(QIODevice::ReadOnly);

                QDomElement MaxFileSize=xml.createElement("MaxFileSize");
                MaxFileSize.appendChild(xml.createTextNode(QString::number(file.size())));

                QDomElement FileLocation=xml.createElement("FileLocation");
                FileLocation.appendChild(xml.createTextNode(path));

                file.close();


            CommandWriteFile.appendChild(FileSystemName);
            CommandWriteFile.appendChild(FileOpenFlagsList);
            CommandWriteFile.appendChild(MaxFileSize);
            CommandWriteFile.appendChild(FileLocation);

            cmd.appendChild(CommandWriteFile);

            //添加到GangCommandsSet节点
            node.appendChild(cmd);

         }
         log("\n");

     }

     {//保存xml文件
         QFile file;
         file.setFileName(outpath);
         file.open(QIODevice::ReadWrite);
         //输出到文件
         QTextStream out_stream(&file);
         xml.save(out_stream,4); //缩进4格


         log("保存GangImageConfig.xml\n\r");
         log("-------------------------------------------------\n");

         file.close();
     }


     {//执行脚本
         foreach(cmd_struct cmd,cmd->getData())
         {
             if(cmd.flags & CMD_FLAGS_AFTER_LIST)
             {
                 if(cmd.cmd.length()>0)
                 {
                     system(cmd.cmd.toStdString().c_str());
                 }
             }
         }
     }

     {//执行BuildGangImage
      log("-------------------------------------------------\n");
      log("执行BuildGangImage\n");
#ifdef WIN32
      BuildThread *t=new BuildThread(QString("BuildGangImage.exe -v -i ")+ui->outputdir->text(),(MainWindow *)this);
      connect(t,SIGNAL(BuildThread_fininshed(bool)),this,SLOT(BuildThread_fininshed(bool)));
        //WinExec((QString("BuildGangImage.exe -v -i ")+ui->outputdir->text()).toStdString().c_str(),SW_SHOW);

         //打开输出文件夹
        //WinExec((QString("explorer ")+ui->outputdir->text().replace("//","/").replace("/","\\")).toStdString().c_str(),SW_SHOW);
#else
#error "暂时只支持window"
#endif
      log("-------------------------------------------------\n");
     }

 }

 void MainWindow::BuildThread_fininshed(bool Is_Normal)
 {
     if(Is_Normal)
     {
         {//执行脚本
             foreach(cmd_struct cmd,cmd->getData())
             {
                 if(cmd.flags & CMD_FLAGS_FINISHED)
                 {
                     if(cmd.cmd.length()>0)
                     {
                         system(cmd.cmd.toStdString().c_str());
                     }
                 }
             }
         }
     }
 }

void MainWindow::on_action_GangImage_triggered()
{
   ConfigXmlGenAndOutput();
}

void MainWindow::log_timer_timeout()
{

    if(log_queue.length()==0) return;
    ui->log->setPlainText(ui->log->toPlainText()+log_queue[0]);
    log_queue.pop_front();
}

void MainWindow::on_action_6_triggered()
{//打开脚本设置窗口
    cmd->show();
}

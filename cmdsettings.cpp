#include "cmdsettings.h"
#include "ui_cmdsettings.h"

#define unused(x) ((void)x)

cmdsettings::cmdsettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::cmdsettings)
{
    ui->setupUi(this);
    setWindowTitle("");
    cmdlist.clear();
}

cmdsettings::~cmdsettings()
{
    delete ui;
}

void cmdsettings::on_cmdlist_itemClicked(QListWidgetItem *item)
{
unused(item);
}

void cmdsettings::on_cmdlist_itemDoubleClicked(QListWidgetItem *item)
{
unused(item);
}

//设置命令列表的数据
void cmdsettings::setData(QList<cmd_struct> _cmdlist)
{
cmdlist.clear();
foreach(cmd_struct cmd,_cmdlist)
{
    cmdlist.push_back(cmd);
}
UpdateData();
}

//更新列表项
void cmdsettings::UpdateData()
{
    ui->cmdlist->clear();
    foreach(cmd_struct cmd,cmdlist)
    {
        ui->cmdlist->addItem(cmd.name);
    }
}

QList<cmd_struct> cmdsettings::getData()
{
    return cmdlist;
}

void cmdsettings::on_pushButton_help_clicked()
{
    QString help="\
帮助文件\n\r\
本程序可以在满足某些条件时执行外部脚本，脚本所需要的参数可通过环境变量获得:\n\r\
INPUTDIR 输入文件夹目录\n\r\
OUTPUTDIR 输出文件夹目录\n\r\
    ";
    QMessageBox::information(this,"帮助",help);
}

void cmdsettings::on_pushButton_new_clicked()
{
    cmd_struct cmd;
    cmd.name=QString::number(cmdlist.length());
    cmd.cmd="";
    cmd.flags=0;
    cmdlist.push_back(cmd);
    UpdateData();
    ui->cmdlist->setCurrentRow(cmdlist.length()-1);
}

void cmdsettings::on_pushButton_delete_clicked()
{
    if(ui->cmdlist->currentRow() < 0 || ui->cmdlist->currentRow() >= cmdlist.length())
    {
        QMessageBox::warning(this,"警告","未选择或者数据异常");
        return;
    }
    int current_index=ui->cmdlist->currentRow();
    for(int i=0;i<cmdlist.length();i++)
    {
        if((i>=current_index) && (i< cmdlist.length()-1))
        {//复制数据但不复制名称
            cmdlist[i].cmd=cmdlist[i+1].cmd;
            cmdlist[i].flags=cmdlist[i+1].flags;
        }
    }
    cmdlist.pop_back();
    UpdateData();
    ui->cmdlist->setCurrentRow(cmdlist.length()-1);
}

void cmdsettings::on_cmdlist_currentRowChanged(int currentRow)
{//更新数据
    if(ui->cmdlist->currentRow() < 0 || ui->cmdlist->currentRow() >= cmdlist.length())
    {
        return;
    }

    //开始更新数据
    ui->lineEdit_name->setText(cmdlist[currentRow].name);
    ui->lineEdit_cmd->setText(cmdlist[currentRow].cmd);

    if(cmdlist[currentRow].flags & CMD_FLAGS_BEFORE_LIST)
    {
        ui->checkBox_1->setCheckState(Qt::CheckState::Checked);
    }
    else
    {
        ui->checkBox_1->setCheckState(Qt::CheckState::Unchecked);
    }

    if(cmdlist[currentRow].flags & CMD_FLAGS_AFTER_LIST)
    {
        ui->checkBox_2->setCheckState(Qt::CheckState::Checked);
    }
    else
    {
        ui->checkBox_2->setCheckState(Qt::CheckState::Unchecked);
    }

    if(cmdlist[currentRow].flags & CMD_FLAGS_FINISHED)
    {
        ui->checkBox_3->setCheckState(Qt::CheckState::Checked);
    }
    else
    {
        ui->checkBox_3->setCheckState(Qt::CheckState::Unchecked);
    }
}

void cmdsettings::on_pushButton_save_clicked()
{//保存数据
    if(ui->cmdlist->currentRow() < 0 || ui->cmdlist->currentRow() >= cmdlist.length())
    {
        QMessageBox::warning(this,"警告","未选择或者数据异常");
        return;
    }

    cmdlist[ui->cmdlist->currentRow()].cmd=ui->lineEdit_cmd->text();
    {
        quint32 flags=0;
        if(ui->checkBox_1->checkState()==Qt::CheckState::Checked)
            flags|=CMD_FLAGS_BEFORE_LIST;
        if(ui->checkBox_2->checkState()==Qt::CheckState::Checked)
            flags|=CMD_FLAGS_AFTER_LIST;
        if(ui->checkBox_3->checkState()==Qt::CheckState::Checked)
            flags|=CMD_FLAGS_FINISHED;
        cmdlist[ui->cmdlist->currentRow()].flags=flags;
    }
}

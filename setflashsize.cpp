#include "setflashsize.h"
#include "ui_setflashsize.h"

SetFlashSize::SetFlashSize(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetFlashSize)
{
    ui->setupUi(this);
    setWindowTitle("设置Flash大小");
    setFixedSize(size());
    flash_size=1024;
}

SetFlashSize::~SetFlashSize()
{
    delete ui;
}

void SetFlashSize::on_lineEdit_textChanged(const QString &arg1)
{
    bool isok=false;
    arg1.toUInt(&isok);
    if(!isok)
    {
       ui->lineEdit->setText("1024");
    }
}

void SetFlashSize::on_pushButton_clicked()
{
    unsigned int size=ui->lineEdit->text().toUInt();
    if(size==0)
    {
        size=1024;
        ui->lineEdit->setText(QString::number(size));
        QMessageBox::warning(this,"警告","容量设置可能有误，设置为默认值1024");
    }
    else
    for(size_t i=0;i<32;i++)
    {
        if(size & (0x01<<i))
         {
          if(size-(0x01<<i) != 0)
          {
              size=1024;
              ui->lineEdit->setText(QString::number(size));
              QMessageBox::warning(this,"警告","容量设置可能有误，设置为默认值1024");
          }
          break;
         }
    }
    flash_size=size;
    setHidden(true);
}

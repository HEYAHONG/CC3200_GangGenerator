#ifndef CMDSETTINGS_H
#define CMDSETTINGS_H

#include <QDialog>
#include <QListWidget>
#include <QMessageBox>

typedef struct
{
    QString name;
    QString cmd;
    quint32 flags;
} cmd_struct;

enum {
  CMD_FLAGS_BEFORE_LIST=(1<<0),
  CMD_FLAGS_AFTER_LIST=(1<<1),
  CMD_FLAGS_FINISHED=(1<<2)
};

namespace Ui {
class cmdsettings;
}

class cmdsettings : public QDialog
{
    Q_OBJECT

public:
    explicit cmdsettings(QWidget *parent = nullptr);
    ~cmdsettings();

    //设置命令列表的数据
    void setData(QList<cmd_struct> _cmdlist);
    //获得命令列表数据
    QList<cmd_struct> getData();

    //更新列表项
    void UpdateData();


private slots:
    void on_cmdlist_itemClicked(QListWidgetItem *item);

    void on_cmdlist_itemDoubleClicked(QListWidgetItem *item);

    void on_pushButton_help_clicked();

    void on_pushButton_new_clicked();

    void on_pushButton_delete_clicked();

    void on_cmdlist_currentRowChanged(int currentRow);

    void on_pushButton_save_clicked();

private:
    Ui::cmdsettings *ui;
    QList<cmd_struct> cmdlist;
};

#endif // CMDSETTINGS_H

#ifndef SETFLASHSIZE_H
#define SETFLASHSIZE_H

#include <QDialog>
#include <QMessageBox>

namespace Ui {
class SetFlashSize;
}

class SetFlashSize : public QDialog
{
    Q_OBJECT

public:
    explicit SetFlashSize(QWidget *parent = nullptr);
    ~SetFlashSize();
    size_t flash_size;

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    void on_pushButton_clicked();

private:
    Ui::SetFlashSize *ui;
};

#endif // SETFLASHSIZE_H

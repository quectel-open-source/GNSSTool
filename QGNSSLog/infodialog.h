#ifndef INFODIALOG_H
#define INFODIALOG_H

#include <QDialog>

namespace Ui {
class infoDialog;
}

class infoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit infoDialog(QWidget *parent = nullptr);
    ~infoDialog();

private slots:
    void on_listWidget_currentRowChanged(int currentRow);

private:
    Ui::infoDialog *ui;
};

#endif // INFODIALOG_H

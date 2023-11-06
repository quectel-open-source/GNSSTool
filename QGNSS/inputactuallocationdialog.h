#ifndef INPUTACTUALLOCATIONDIALOG_H
#define INPUTACTUALLOCATIONDIALOG_H

#include <QDialog>
#include "CEP_calc.h"

namespace Ui {
class InputActualLocationDialog;
}

class InputActualLocationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputActualLocationDialog(QWidget *parent = nullptr);
    ~InputActualLocationDialog();

    LLA lla;

private slots:
    void on_buttonBox_accepted();

private:
    Ui::InputActualLocationDialog *ui;
    void loadSettingData(bool rw = false);//default read data
};

#endif // INPUTACTUALLOCATIONDIALOG_H

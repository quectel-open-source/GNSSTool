#ifndef AGNSS_LC29Y_ONLINE_DIALOG_H
#define AGNSS_LC29Y_ONLINE_DIALOG_H

#include <QDialog>

namespace Ui {
class AGNSS_LC29Y_Online_Dialog;
}

class AGNSS_LC29Y_Online_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit AGNSS_LC29Y_Online_Dialog(QWidget *parent = nullptr);
    ~AGNSS_LC29Y_Online_Dialog();

private slots:
    void on_checkBox_useSysTime_stateChanged(int arg1);

    void on_checkBox_useCurPos_stateChanged(int arg1);

    void on_pushButton_selectFile_clicked();

    void on_pushButton_transfer_clicked();

private:
    Ui::AGNSS_LC29Y_Online_Dialog *ui;

    void sendUTC();
    void sendPostion();
    void sendEPH();
};

#endif // AGNSS_LC29Y_ONLINE_DIALOG_H

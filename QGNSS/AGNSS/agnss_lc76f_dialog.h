#ifndef AGNSS_LC76F_DIALOG_H
#define AGNSS_LC76F_DIALOG_H

#include <QDialog>

namespace Ui {
class AGNSS_LC76F_Dialog;
}

class AGNSS_LC76F_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit AGNSS_LC76F_Dialog(QWidget *parent = nullptr);
    ~AGNSS_LC76F_Dialog();
    void addGPD_Checksum(QByteArray &cmd);
    bool waitACK(uint8_t type = 0);
    void sendAGNSS_File();

private slots:

    void on_pushButton_selectFile_clicked();

    void on_checkBox_useCurPos_stateChanged(int arg1);

    void on_checkBox_useCurTime_stateChanged(int arg1);

    void on_pushButton_transfer_clicked();

    void on_pushButton_download_clicked();

    void on_pushButton_clear_clicked();

private:
    Ui::AGNSS_LC76F_Dialog *ui;
};

#endif // AGNSS_LC76F_DIALOG_H

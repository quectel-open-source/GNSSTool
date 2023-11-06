#ifndef AGNSS_LC79D_DIALOG_H
#define AGNSS_LC79D_DIALOG_H

#define  NMEA_FWUPGRADE_COMMAND_STRING       "$PQLTODOWNLOAD\r\n"
#define  NMEA_FWUPGRADE_RESPOND_STRING       "$PQLTODOWNLOADOK*46"
#define  LTO_FILE_MAX_SIZE 1024*1024

#include <QDialog>

namespace Ui {
class AGNSS_LC79D_Dialog;
}

class AGNSS_LC79D_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit AGNSS_LC79D_Dialog(QWidget *parent = nullptr);
    ~AGNSS_LC79D_Dialog();

private slots:
    void on_pushButton_transfer_clicked();

    void on_pushButton_selectFile_clicked();

    void on_pushButton_download_clicked();

    void on_checkBox_useCurPos_stateChanged(int arg1);

    void on_checkBox_useCurTime_stateChanged(int arg1);

private:
    Ui::AGNSS_LC79D_Dialog *ui;
    void downloadLTO_DataToModule();
    void sendTime();
    void sendPosition();
};

#endif // AGNSS_LC79D_DIALOG_H

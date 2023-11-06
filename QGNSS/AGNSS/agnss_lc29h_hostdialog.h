#ifndef AGNSS_LC29H_HOSTDIALOG_H
#define AGNSS_LC29H_HOSTDIALOG_H

#define UTC_DATE_FORMAT "yyyy-MM-dd hh:mm:ss"

#define EPO_GPS_ONLY_FILE_NAME  "./AGNSS_File/EPO_GPS_3_1.DAT"
#define EPO_GR_ONLY_FILE_NAME  "./AGNSS_File/EPO_GR_3_1.DAT"
#define EPO_GAL_ONLY_FILE_NAME  "./AGNSS_File/EPO_GAL_3.DAT"
#define EPO_BD_ONLY_FILE_NAME  "./AGNSS_File/EPO_BDS_3.DAT"

#define QEPO_GPS_FILE_NAME "./AGNSS_File/QGPS.DAT"
#define QEPO_GLONASS_FILE_NAME "./AGNSS_File/QG_R.DAT"
#define QEPO_GAL_FILE_NAME "./AGNSS_File/QGA.DAT"
#define QEPO_BDS_FILE_NAME "./AGNSS_File/QBD2.DAT"



#include <QDialog>
#include "ftp_manager.h"
#include "QPlainTextEdit"

namespace Ui {
class AGNSS_LC29H_hostDialog;
}

class AGNSS_LC29H_hostDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AGNSS_LC29H_hostDialog(QWidget *parent = nullptr);
    ~AGNSS_LC29H_hostDialog();

    static bool getEPO_6h_File(FTP_Manager &manager, QPlainTextEdit *msgWindow = nullptr);
    static bool createQEPO_File(char type, QString fileName);
    static QString sendUTC(QDateTime utc);
    static QString sendPosition(double lat, double lon, double height);
    static QString sendHostEPO(QString fileName, uint8_t type);

private slots:
    void on_checkBox_useCurPos_stateChanged(int arg1);

    void on_checkBox_useCurTime_stateChanged(int arg1);

    void on_pushButton_transfer_clicked();

//    void on_pushButton_HEPO_clicked();

private:
    Ui::AGNSS_LC29H_hostDialog *ui;
    FTP_Manager manager;

    QString transfer_AGNSS_Data();
};

#endif // AGNSS_LC29H_HOSTDIALOG_H

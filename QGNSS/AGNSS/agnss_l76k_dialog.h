#ifndef AGNSS_L76K_DIALOG_H
#define AGNSS_L76K_DIALOG_H

#include <QDialog>

namespace Ui {
class AGNSS_L76K_Dialog;
}

class AGNSS_L76K_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit AGNSS_L76K_Dialog(QWidget *parent = nullptr);
    ~AGNSS_L76K_Dialog();

    typedef struct{
        double Lat;
        double Lon;
        double Alt;
        double TOW;
        float FreqBias;
        float pAcc;
        float tAcc;
        float fACC;
        uint32_t Res;
        uint16_t WN;
        uint8_t TimerSource;
        uint8_t posVaild:1;//B0
        uint8_t timeVaild:1;//B1
        uint8_t :1;//B2
        uint8_t :1;//B3
        uint8_t :1;//B4
        uint8_t LLA_format:1;//B5
        uint8_t altInvaild:1;//B6
        uint8_t :1;//B7
    }AID_INI;

private slots:
    void on_checkBox_position_stateChanged(int arg1);

    void on_checkBox_time_stateChanged(int arg1);

    void on_checkBox_freqBias_stateChanged(int arg1);

    void on_checkBox_llaFormat_stateChanged(int arg1);

    void on_checkBox_fAcc_stateChanged(int arg1);

    void on_pushButton_selectFile_clicked();

    void on_pushButton_transfer_clicked();

    void on_checkBox_useCurPos_stateChanged(int arg1);

    void on_checkBox_useCurTime_stateChanged(int arg1);

private:
    Ui::AGNSS_L76K_Dialog *ui;
    int sendAGNSS_dataFile();
};

#endif // AGNSS_L76K_DIALOG_H

#ifndef HD8120_OFFLINE_DIALOG_H
#define HD8120_OFFLINE_DIALOG_H

#include <QDialog>

#include "YmodemFileTransmit.h"

namespace Ui {
class HD8120_offline_Dialog;
}

class HD8120_offline_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit HD8120_offline_Dialog(QWidget *parent = nullptr);
    ~HD8120_offline_Dialog();

private slots:
    void transmitProgress(int progress);
    void transmitStatus(YmodemFileTransmit::Status status);

    void on_checkBox_useSysTime_stateChanged(int arg1);

    void on_checkBox_useCurPos_stateChanged(int arg1);

    void on_pushButton_selectFile_clicked();

    void on_pushButton_send_clicked();

    void on_pushButton_Download_clicked();

private:
    Ui::HD8120_offline_Dialog *ui;
    YmodemFileTransmit *ymodemFileTransmit;
    bool transmitButtonStatus = false;

    void sendUTC();
    void sendPostion();
    bool createEPH_File(QString fileName, QString &newFile);
};

#endif // HD8120_OFFLINE_DIALOG_H

#ifndef AGNSS_LC29H_DIALOG_H
#define AGNSS_LC29H_DIALOG_H

#include <QDialog>
#include "QProgressBar"
#include "QButtonGroup"
#include "QFile"

namespace Ui {
class AGNSS_LC29H_Dialog;
}

class AGNSS_LC29H_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit AGNSS_LC29H_Dialog(QWidget *parent = nullptr);
    ~AGNSS_LC29H_Dialog();
    static QString sendEPO_File(QString AGNSS_FileName, QProgressBar *progressBar = nullptr, const char type = 'G');
    static QString send_Only_EPO(QFile &AGNSS_File, QProgressBar *progressBar = nullptr, char type = 'G');
    static QString send_GR_EPO(QFile &AGNSS_File, QProgressBar *progressBar = nullptr);

private slots:
    void on_pushButton_selectFile_clicked();

    void on_pushButton_download_clicked();

    void on_checkBox_useCurTime_stateChanged(int arg1);

    void on_checkBox_useCurPos_stateChanged(int arg1);

    void on_pushButton_transfer_clicked();

private:
    Ui::AGNSS_LC29H_Dialog *ui;
    QButtonGroup btnSatelliteType;

    QString transfer();
};

#endif // AGNSS_LC29H_DIALOG_H

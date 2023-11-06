#include "agnss_lc29h_dialog.h"
#include "ui_agnss_lc29h_dialog.h"
#include "agnss_lc29h_hostdialog.h"
#include "mainwindow.h"
#include "CommonEPO_AG3335.h"
#include <QFileDialog>

AGNSS_LC29H_Dialog::AGNSS_LC29H_Dialog(QWidget *parent) : QDialog(parent),
                                                          ui(new Ui::AGNSS_LC29H_Dialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("EPO Download");
    ui->ftp_client->setFTP_info("QEPO", "123456");
    ui->progressBar_download->setValue(0);
    // add button to ButtonGroup
    btnSatelliteType.addButton(ui->radioButton_GPS, 'G');
    btnSatelliteType.addButton(ui->radioButton_Beidou, 'C');
    btnSatelliteType.addButton(ui->radioButton_GG, 'R');
    btnSatelliteType.addButton(ui->radioButton_Galileo, 'E');
}

AGNSS_LC29H_Dialog::~AGNSS_LC29H_Dialog()
{
    delete ui;
}

QString AGNSS_LC29H_Dialog::sendEPO_File(QString AGNSS_FileName, QProgressBar *progressBar, const char type)
{
    QByteArray data;
    QByteArray nmea_cmd;
    QFile EPOfile(AGNSS_FileName);
    QString str;
    if (EPOfile.size() == 0)
    {
        return "Error in file format !";
    }
    if (!EPOfile.open(QIODevice::ReadOnly))
    {
        return "File open failed !";
    }
    //    mainForm->writeData("$PAIR472*3B\r\n");
    //    qgnss_sleep(120);
    switch (type)
    {
    case 'G':
    {
        send_Only_EPO(EPOfile, progressBar, type);
        break;
    }
    case 'C':
    {
        EPOfile.read(72);
        send_Only_EPO(EPOfile, progressBar, type);
        break;
    }
    case 'R':
    {
        send_GR_EPO(EPOfile, progressBar);
        break;
    }
    case 'E':
    {
        EPOfile.read(72);
        send_Only_EPO(EPOfile, progressBar, type);
        break;
    }
    }
    return "";
}

QString AGNSS_LC29H_Dialog::send_Only_EPO(QFile &AGNSS_File, QProgressBar *progressBar, char type)
{
    char buffer[512];
    char readBuffer[512];
    uint16_t length = 0;
    QByteArray data;
    length = gnss_epo_encode_binary(1200, buffer, 512, &type, 1);
    mainForm->sendData(QByteArray(buffer, length));
    qgnss_sleep(120);
    //    if(type != 'G')
    //        AGNSS_File.read(buffer,72);
    while (AGNSS_File.read(readBuffer, 72))
    {
        length = gnss_epo_encode_binary(1201, buffer, 512, readBuffer, 72);
        mainForm->sendData(QByteArray(buffer, length));
        progressBar->setValue(AGNSS_File.pos() * 100 / AGNSS_File.size());
        qgnss_sleep(30);
    }
    length = gnss_epo_encode_binary(1202, buffer, 512, &type, 1);
    mainForm->sendData(QByteArray(buffer, length));
    return "";
}

QString AGNSS_LC29H_Dialog::send_GR_EPO(QFile &AGNSS_File, QProgressBar *progressBar)
{
    char buffer[512];
    char readBuffer[512];
    uint32_t sv = 0;
    uint16_t length = 0;
    // type is the GPS
    char type = 'G';
    length = gnss_epo_encode_binary(1200, buffer, 512, &type, 1);
    mainForm->sendData(QByteArray(buffer, length));
    qgnss_sleep(120);
    while (AGNSS_File.read(readBuffer, 72))
    {
        length = gnss_epo_encode_binary(1201, buffer, 512, readBuffer, 72);
        mainForm->sendData(QByteArray(buffer, length));
        sv++;
        progressBar->setValue(AGNSS_File.pos() * 100 / AGNSS_File.size() * 0.6);
        qgnss_sleep(30);
        if (!(sv % 32))
        {
            AGNSS_File.read(72 * 24);
        }
    }
    length = gnss_epo_encode_binary(1202, buffer, 512, &type, 1);
    mainForm->sendData(QByteArray(buffer, length));
    qgnss_sleep(120);

    // type is the GLONASS
    AGNSS_File.seek(0);
    type = 'R';
    length = gnss_epo_encode_binary(1200, buffer, 512, &type, 1);
    mainForm->sendData(QByteArray(buffer, length));
    qgnss_sleep(120);
    QByteArray data;
    while (1)
    {
        data = AGNSS_File.read(72 * 32);
        if (!data.size())
        {
            break;
        }
        for (int i = 0; i < 24; i++)
        {
            if (!AGNSS_File.read(readBuffer, 72))
            {
                break;
            }
            length = gnss_epo_encode_binary(1201, buffer, 512, readBuffer, 72);
            mainForm->sendData(QByteArray(buffer, length));
            progressBar->setValue(AGNSS_File.pos() * 100 / AGNSS_File.size() * 0.4 + 60);
            qgnss_sleep(30);
        }
    }
    length = gnss_epo_encode_binary(1202, buffer, 512, &type, 1);
    mainForm->sendData(QByteArray(buffer, length));
    qgnss_sleep(120);
    return "";
}

void AGNSS_LC29H_Dialog::on_pushButton_selectFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Select File!", "./AGNSS_File");
    ui->lineEdit_filePath->setText(fileName);
}

void AGNSS_LC29H_Dialog::on_pushButton_download_clicked()
{
    ui->pushButton_download->setEnabled(false);
    QString msg = sendEPO_File(ui->lineEdit_filePath->text(), ui->progressBar_download, btnSatelliteType.checkedId());
    if (msg != "")
    {
        QMessageBox::warning(this, "Warning", msg, QMessageBox::Ok);
        ui->progressBar_download->setValue(0);
    }
    ui->pushButton_download->setEnabled(true);
}

void AGNSS_LC29H_Dialog::on_checkBox_useCurTime_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    QDateTime UTC(QDateTime::currentDateTime().toUTC()); // current UTC time
    ui->lineEdit_UTC->setText(UTC.toString(UTC_DATE_FORMAT));
}

void AGNSS_LC29H_Dialog::on_checkBox_useCurPos_stateChanged(int arg1)
{
    if (arg1 == Qt::Checked)
    {
        if (mainForm->NMEA_Data.Solution_Data.savedData_L.isEmpty())
            return;
        QString tempStr;
        tempStr = QString::number(mainForm->NMEA_Data.Solution_Data.savedData_L.last().Longitude, 'f', 8);
        ui->lineEdit_long->setText(tempStr);
        tempStr = QString::number(mainForm->NMEA_Data.Solution_Data.savedData_L.last().Latitude, 'f', 8);
        ui->lineEdit_lat->setText(tempStr);
        tempStr = QString::number(mainForm->NMEA_Data.Solution_Data.savedData_L.last().AltitudeMSL, 'f', 2);
        ui->lineEdit_alt->setText(tempStr);
    }
    else if (arg1 == Qt::Unchecked)
    {
        ui->lineEdit_long->clear();
        ui->lineEdit_lat->clear();
        ui->lineEdit_alt->clear();
    }
}

void AGNSS_LC29H_Dialog::on_pushButton_transfer_clicked()
{
    ui->pushButton_transfer->setEnabled(false);
    QString msg = transfer();
    if (msg != "")
    {
        QMessageBox::warning(this, "Warning", msg, QMessageBox::Ok);
    }
    ui->pushButton_transfer->setEnabled(true);
}

QString AGNSS_LC29H_Dialog::transfer()
{
    // send restart cmd
    if (ui->comboBox_restartMode->currentIndex())
    {
        mainForm->sendResetCMD(ui->comboBox_restartMode->currentIndex());
    }

    // send UTC
    //    ui->plainTextEdit->appendPlainText("send UTC.");
    if (ui->groupBox_time->isChecked())
    {
        if (ui->checkBox_useCurTime->isChecked())
        {
            on_checkBox_useCurTime_stateChanged(true);
        }
        AGNSS_LC29H_hostDialog::sendUTC(QDateTime::fromString(ui->lineEdit_UTC->text(), UTC_DATE_FORMAT));
    }

    // send position
    //    ui->plainTextEdit->appendPlainText("send position.");
    if (ui->groupBox_position->isChecked())
    {
        AGNSS_LC29H_hostDialog::sendPosition(ui->lineEdit_lat->text().toDouble(),
                                             ui->lineEdit_long->text().toDouble(),
                                             ui->lineEdit_alt->text().toDouble());
    }

    // send host EPO
    //    ui->plainTextEdit->appendPlainText("send host EPO.");
    //    QString msg = sendHostEPO(QEPO_GPS_FILE_NAME, EPO_DEMO_MODE_GPS);
    //    if(msg != ""){
    //        return msg;
    //    }
    //    msg = sendHostEPO(QEPO_GLONASS_FILE_NAME, EPO_DEMO_MODE_GLONASS);
    //    if(msg != ""){
    //        return msg;
    //    }
    //    ui->plainTextEdit->appendPlainText("*******Success*******");
    return "";
}

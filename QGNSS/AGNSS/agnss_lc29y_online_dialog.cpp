#include "agnss_lc29y_online_dialog.h"
#include "ui_agnss_lc29y_online_dialog.h"
#include "mainwindow.h"
#include "allystar_binary_msg.h"

using namespace qgnss_allstar_msg;

AGNSS_LC29Y_Online_Dialog::AGNSS_LC29Y_Online_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AGNSS_LC29Y_Online_Dialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setWindowTitle("Assistant GNSS");
    ui->ftp_client->setFTP_addr("agnss.allystar.com");
    ui->ftp_client->setFTP_info("quectel","Agnss22@qt957");
    ui->dateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    ui->progressBar_download->setValue(0);
}

AGNSS_LC29Y_Online_Dialog::~AGNSS_LC29Y_Online_Dialog()
{
    delete ui;
}

void AGNSS_LC29Y_Online_Dialog::on_checkBox_useSysTime_stateChanged(int arg1)
{
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTimeUtc());
    ui->dateTimeEdit->setEnabled(!arg1);
}

void AGNSS_LC29Y_Online_Dialog::on_checkBox_useCurPos_stateChanged(int arg1)
{
    if(arg1 == Qt::Checked){
        if(mainForm->NMEA_Data.Solution_Data.savedData_L.isEmpty()) return;
        QString tempStr;
        tempStr = QString::number(mainForm->NMEA_Data.Solution_Data.savedData_L.last().Longitude, 'f', 8);
        ui->lineEdit_long->setText(tempStr);
        tempStr = QString::number(mainForm->NMEA_Data.Solution_Data.savedData_L.last().Latitude, 'f', 8);
        ui->lineEdit_lat->setText(tempStr);
        tempStr = QString::number(mainForm->NMEA_Data.Solution_Data.savedData_L.last().AltitudeMSL, 'f', 2);
        ui->lineEdit_alt->setText(tempStr);
    }else if(arg1 == Qt::Unchecked){
        ui->lineEdit_long->clear();
        ui->lineEdit_lat->clear();
        ui->lineEdit_alt->clear();
    }
}

void AGNSS_LC29Y_Online_Dialog::on_pushButton_selectFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Select File!","./AGNSS_File");
    ui->lineEdit_filePath->setText(fileName);
}

void AGNSS_LC29Y_Online_Dialog::on_pushButton_transfer_clicked()
{
    try {
        ui->pushButton_transfer->setEnabled(false);
        mainForm->sendResetCMD(ui->comboBox_restartMode->currentIndex());
        ui->progressBar_download->setValue(10);
        if(ui->groupBox_utc->isChecked()){
            //send utc
            sendUTC();
        }
        if(ui->groupBox_postion->isChecked()){
            //send postion
            sendPostion();
        }
        if(ui->groupBox_file->isChecked()){
            //send file
            sendEPH();
        }
        ui->progressBar_download->setValue(100);
    } catch (QString msg) {
        ui->progressBar_download->setValue(0);
        QMessageBox::warning(this, "Failed", msg, QMessageBox::Cancel);
        qDebug()<<msg;
    }
    ui->pushButton_transfer->setEnabled(true);
}

void AGNSS_LC29Y_Online_Dialog::sendUTC()
{
    if(ui->checkBox_useSysTime->isChecked()){
        ui->dateTimeEdit->setDateTime(QDateTime::currentDateTimeUtc());
    }
    QByteArray cmd = QByteArray::fromHex("F1 D9 0B 11 14 00 00 00");
    AidingUTC utc;
    utc.day = ui->dateTimeEdit->date().day();
    utc.year = ui->dateTimeEdit->date().year();
    utc.month = ui->dateTimeEdit->date().month();
    utc.hour = ui->dateTimeEdit->time().hour();
    utc.minute = ui->dateTimeEdit->time().minute();
    utc.second = ui->dateTimeEdit->time().second();
    utc.leap_sec = ui->lineEdit_leap_sec->text().toUInt();
    cmd.append(reinterpret_cast<char *>(&utc), sizeof(utc));
    addAllystarBinaryCS(cmd);
    mainForm->sendData(cmd);
    qDebug()<<cmd.toHex(' ').toUpper();
}

void AGNSS_LC29Y_Online_Dialog::sendPostion()
{
    QByteArray cmd = QByteArray::fromHex("F1 D9 0B 10 11 00 01");
    AidingPos pos;
    pos.lat = ui->lineEdit_lat->text().toFloat() * 1e7;
    pos.lon = ui->lineEdit_long->text().toFloat() * 1e7;
    pos.alt = ui->lineEdit_alt->text().toFloat() * 100;
    pos.pos_acc = ui->lineEdit_posAcy->text().toUInt();
    cmd.append(reinterpret_cast<char *>(&pos), sizeof(pos));
    addAllystarBinaryCS(cmd);
    mainForm->sendData(cmd);
    qDebug()<<cmd.toHex(' ').toUpper();
}

void AGNSS_LC29Y_Online_Dialog::sendEPH()
{
    qDebug()<<"send EPH";
    QFile ephFile(ui->lineEdit_filePath->text());

    if(!ephFile.open(QIODevice::ReadOnly))
        throw ephFile.errorString();
    if(ephFile.size() > 5120){
        throw QString("The file is too large !");
    }
    QByteArray data = ephFile.readAll();
    QByteArray singleEPH;
    qDebug()<<data.size()<<singleEPH.size();
    int msgLength = 0;
    int progress = 0;
    foreach (auto var, data) {
        progress++;
        if(singleEPH.size()){
            singleEPH.append(var);
            if(static_cast<uint8_t>(singleEPH.at(1)) != 0xd9)
                singleEPH.clear();
            if(singleEPH.size() == 6){
                msgLength = (static_cast<uint32_t>(singleEPH.at(5)) << 8) + static_cast<uint8_t>(singleEPH.at(4));
                if(msgLength > 1024){
                    throw QString("Error in file format !");
                }
            }
            if(singleEPH.size() == msgLength + 8){
                mainForm->sendData(singleEPH);
                ui->progressBar_download->setValue(progress*100.0/data.size());
                qgnss_sleep(50);
                msgLength = 0;
                singleEPH.clear();
            }
        }
        else if(static_cast<uint8_t>(var) == 0xf1){
            singleEPH.append(var);
        }
    }
}

#include "agnss_l76k_dialog.h"
#include "ui_agnss_l76k_dialog.h"
#include "QFileDialog"
#include "mainwindow.h"
#include "Configuration/configuration_l76k.h"
#include "DataParser/casic_parser.h"

AGNSS_L76K_Dialog::AGNSS_L76K_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AGNSS_L76K_Dialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("AGNSS L76K");
    ui->ftp_client->setFTP_info("iconfchinaAGPSDemo","123456");
    ui->progressBar_download->setValue(0);
}

AGNSS_L76K_Dialog::~AGNSS_L76K_Dialog()
{
    delete ui;
}

void AGNSS_L76K_Dialog::on_checkBox_position_stateChanged(int arg1)
{
    ui->lineEdit_long->setEnabled(arg1);
    ui->lineEdit_alt->setEnabled(arg1);
    ui->lineEdit_lat->setEnabled(arg1);
    ui->checkBox_useCurPos->setEnabled(arg1);
    ui->checkBox_useCurPos->setCheckState(Qt::Unchecked);
}

void AGNSS_L76K_Dialog::on_checkBox_time_stateChanged(int arg1)
{
    ui->lineEdit_wn->setEnabled(arg1);
    ui->lineEdit_tow->setEnabled(arg1);
    ui->checkBox_useCurTime->setEnabled(arg1);
}

void AGNSS_L76K_Dialog::on_checkBox_freqBias_stateChanged(int arg1)
{
    ui->lineEdit_freqBias->setEnabled(arg1);
}

void AGNSS_L76K_Dialog::on_checkBox_llaFormat_stateChanged(int arg1)
{

}

void AGNSS_L76K_Dialog::on_checkBox_fAcc_stateChanged(int arg1)
{
    ui->lineEdit_fAcc->setEnabled(arg1);
}

void AGNSS_L76K_Dialog::on_pushButton_selectFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Select File!","./AGNSS_File");
    ui->lineEdit_filePath->setText(fileName);
}

void AGNSS_L76K_Dialog::on_pushButton_transfer_clicked()
{
    if(ui->comboBox_restartMode->currentIndex()){
        mainForm->sendResetCMD(ui->comboBox_restartMode->currentIndex());
    }
//    ui->pushButton_transfer->setEnabled(false);
    //send AID-INI
    QByteArray cmd;
    AID_INI init = {};
    if(ui->checkBox_position->isChecked()){
        init.posVaild = true;
        init.Alt = ui->lineEdit_alt->text().toDouble();
        init.Lon = ui->lineEdit_long->text().toDouble();
        init.Lat = ui->lineEdit_lat->text().toDouble();
    }
    if(ui->checkBox_time->isChecked()){
        init.timeVaild = true;
        if(ui->checkBox_useCurTime->isChecked()){
            on_checkBox_useCurTime_stateChanged(Qt::Checked);
        }
        init.WN = ui->lineEdit_wn->text().toUShort();
        init.TOW = ui->lineEdit_tow->text().toDouble();
    }
    init.LLA_format = ui->checkBox_llaFormat->isChecked();
    init.altInvaild = ui->checkBox_Alt_Invalid->isChecked();
    cmd.append(0xBA);
    cmd.append(0xCE);
    cmd.append(sizeof(AID_INI));
    cmd.append(static_cast<char>(0));
    cmd.append(0x0B);
    cmd.append(0x01);
    cmd.append((char *)(&init),sizeof(AID_INI));
    configuration_L76K::addChecksum(cmd);
    qDebug()<<"AID_INI"<<cmd.toHex(' ').toUpper();
    mainForm->sendAndWaitForACK(cmd,CASIC_Parser::getInstance()->casic_data.ACK);
    //send eph
    sendAGNSS_dataFile();
}

void AGNSS_L76K_Dialog::on_checkBox_useCurPos_stateChanged(int arg1)
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

void AGNSS_L76K_Dialog::on_checkBox_useCurTime_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    QDateTime UTC(QDateTime::currentDateTime().toUTC());//current UTC time
    QString str;
    //1980-01-06(315964800)      18 is leap seconds
    qint64 gpsSecs = UTC.toSecsSinceEpoch() - 315964800 + 18;
    //GPS week
    str = QString::number(gpsSecs/(3600*7*24));
    ui->lineEdit_wn->setText(str);
    //TOW
    str = QString::number(gpsSecs%(3600*7*24));
    ui->lineEdit_tow->setText(str);
}

int AGNSS_L76K_Dialog::sendAGNSS_dataFile()
{
    QFile ephFile(ui->lineEdit_filePath->text());
    QByteArray head;
    head.append(0xBA);
    head.append(0xCE);
    if(ephFile.open(QIODevice::ReadOnly)){
        if(ephFile.readLine().contains("CASIC")){
            QByteArray ephData = ephFile.readAll();
            int frist = ephData.indexOf(head);
            int sec = ephData.indexOf(head,frist+2);
            if(!mainForm->sendAndWaitForACK(ephData.mid(frist,sec-frist),
                                        CASIC_Parser::getInstance()->casic_data.ACK)){
                ui->progressBar_download->setValue(0);
                QMessageBox::warning(this, "Download error", "No reply from the module!", QMessageBox::Cancel);
                return 0;
            }
            while(sec > 0){
                frist = sec;
                sec = ephData.indexOf(head,frist+2);
                if(!mainForm->sendAndWaitForACK(ephData.mid(frist,sec-frist),
                                            CASIC_Parser::getInstance()->casic_data.ACK)){
                    ui->progressBar_download->setValue(0);
                    QMessageBox::warning(this, "Download error", "No reply from the module!", QMessageBox::Cancel);
                    return 0;
                }
                ui->progressBar_download->setValue(sec*100.0/ephFile.size());
            }
            ui->progressBar_download->setValue(100);
        }
        else{
            QMessageBox::warning(this, "File error", "This is not an CASIC AGNSS file!", QMessageBox::Cancel);
            return 0;
        }
    }else{
        QMessageBox::warning(this, "File open failed", ephFile.errorString(), QMessageBox::Cancel);
        return 0;
    }
    return 1;
}


#include "agnss_lc76f_dialog.h"
#include "ui_agnss_lc76f_dialog.h"
#include "QMessageBox"
#include "mainwindow.h"
#include "common.h"

AGNSS_LC76F_Dialog::AGNSS_LC76F_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AGNSS_LC76F_Dialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("AGNSS LC76F");
    ui->ftp_client->setFTP_info("goke-dat","123456");
    ui->progressBar_download->setValue(0);
}

AGNSS_LC76F_Dialog::~AGNSS_LC76F_Dialog()
{
    delete ui;
}

void AGNSS_LC76F_Dialog::addGPD_Checksum(QByteArray &cmd)
{
    uint8_t checksum = cmd[2];
    for(int i = 3;i < cmd.size();i++){
        checksum = checksum ^ cmd[i];
    }
    cmd.append(checksum);
}

bool AGNSS_LC76F_Dialog::waitACK(uint8_t type)
{
    Q_UNUSED(type)
    qgnss_sleep(1000);
    QByteArray data = mainForm->m_serial->readAll();
    QByteArray gokeHead = QByteArray::fromHex("AA F0");
    if(data.contains(gokeHead))
        return true;
    return false;
}

void AGNSS_LC76F_Dialog::sendAGNSS_File()
{
    try {
        QFile ephFile(ui->lineEdit_filePath->text());
        if(!ephFile.open(QIODevice::ReadOnly))
            throw ephFile.errorString();
        disconnect(mainForm->m_serial, &QSerialPort::readyRead, mainForm, &MainWindow::ReadDataFromSerial);
        mainForm->m_serial->readAll();
        ui->progressBar_download->setValue(10);
        //change BINARY to NMEA
        QByteArray cmd("$PGKC149,1,");
//        cmd.append(QString::number(mainForm->portInfo.baudRate));
        cmd.append(mainForm->dev_info.stringBaudRate);
        addNMEA_CS(cmd);
        mainForm->sendData(cmd);
        if(!waitACK())
            throw QString("No reply!Please restart the module.");
        ui->progressBar_download->setValue(30);
        //send eph file
        QByteArray ephData = ephFile.readAll();
        for(int i = 0;i <= ephData.size()/512; i++){
            cmd.clear();
            cmd = QByteArray::fromHex("aa f0 0b 02 66 02");
            cmd.append(i);
            cmd.append(i>>8);
            QByteArray ephSegment = ephData.mid(i*512,512);
            cmd.append(ephSegment);
            if(512 - ephSegment.size() > 0)
                cmd.append(512- ephSegment.size(),0);
            addGPD_Checksum(cmd);cmd.append(0x0D);cmd.append(0x0A);
            mainForm->sendData(cmd);
            if(!waitACK())
                throw QString("No reply!Please restart the module.");
        }
        ui->progressBar_download->setValue(80);
        //send the transmission end  cmd
        cmd.clear();
        cmd = QByteArray::fromHex("aa f0 0b 00 66 02 ff ff");
        addGPD_Checksum(cmd);cmd.append(0x0D);cmd.append(0x0A);
        mainForm->sendData(cmd);
        if(!waitACK())
            throw QString("No reply! Please restart the module.");
        ui->progressBar_download->setValue(90);
        //change BINARY to NMEA
        cmd.clear();
        cmd = QByteArray::fromHex("aa f0 0e 00 95 00 00");
        cmd.append(mainForm->dev_info.baudRate);
        cmd.append(mainForm->dev_info.baudRate >> 8);
        cmd.append(mainForm->dev_info.baudRate >> 16);
        cmd.append(mainForm->dev_info.baudRate >> 24);
        addGPD_Checksum(cmd);cmd.append(0x0D);cmd.append(0x0A);
        mainForm->sendData(cmd);
        if(!waitACK())
            throw QString("No reply! Please restart the module.");
        ui->progressBar_download->setValue(100);
    } catch (QString msg) {
        ui->progressBar_download->setValue(0);
        QMessageBox::warning(this, "Failed", msg, QMessageBox::Cancel);
        qDebug()<<msg;
    }catch (...) {
        QMessageBox::warning(this, "Failed", "Unknown error", QMessageBox::Cancel);
    }
    connect(mainForm->m_serial, &QSerialPort::readyRead, mainForm, &MainWindow::ReadDataFromSerial);
}

void AGNSS_LC76F_Dialog::on_pushButton_selectFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Select File!","./AGNSS_File");
    ui->lineEdit_filePath->setText(fileName);
}

void AGNSS_LC76F_Dialog::on_checkBox_useCurPos_stateChanged(int arg1)
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

void AGNSS_LC76F_Dialog::on_checkBox_useCurTime_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    QDateTime UTC(QDateTime::currentDateTime().toUTC());//current UTC time
    ui->lineEdit_UTC->setText(UTC.toString("yyyy-MM-dd hh:mm:ss"));
}

void AGNSS_LC76F_Dialog::on_pushButton_transfer_clicked()
{
    if(ui->comboBox_restartMode->currentIndex()){
        mainForm->sendResetCMD(ui->comboBox_restartMode->currentIndex());
        qgnss_sleep(1000);
    }
//    if(ui->checkBox_sendFile->isChecked())
//        sendAGNSS_File();
    if(ui->checkBox_useCurTime->isChecked())
        on_checkBox_useCurTime_stateChanged(0);
    QList<QString> time,date;
    date = ui->lineEdit_UTC->text().split(' ');
    if(date.size () >1)
    {
        //Extract time first,
        time = date[1].split(':');
        date = date[0].split('-');
    }
    if(date.size()>2 && time.size()>2)
    {
        QString cmd = QString("$PGKC639,%1,%2,%3,%4,%5,%6,%7,%8,%9")
                .arg(ui->lineEdit_lat->text())
                .arg(ui->lineEdit_long->text())
                .arg(ui->lineEdit_alt->text())
                .arg(date[0])
                .arg(date[1])
                .arg(date[2])
                .arg(time[0])
                .arg(time[1])
                .arg(time[2]);
        QByteArray CMD = cmd.toLocal8Bit();
        addNMEA_CS(CMD);
        mainForm->sendData(CMD);
        qDebug()<<CMD;
    }
    else{
        QMessageBox::warning(this, "Failed", "Incorrect location or time format.", QMessageBox::Cancel);
    }
}

void AGNSS_LC76F_Dialog::on_pushButton_download_clicked()
{
    ui->pushButton_download->setEnabled(false);
    sendAGNSS_File();
    ui->pushButton_download->setEnabled(true);
}

void AGNSS_LC76F_Dialog::on_pushButton_clear_clicked()
{
    mainForm->sendData("$PGKC040*2B\r\n");
}

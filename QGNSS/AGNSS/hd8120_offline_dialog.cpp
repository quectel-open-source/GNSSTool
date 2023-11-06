#include "hd8120_offline_dialog.h"
#include "ui_hd8120_offline_dialog.h"

#include "common.h"
#include "mainwindow.h"
#include "allystar_binary_msg.h"

using namespace qgnss_allstar_msg;

HD8120_offline_Dialog::HD8120_offline_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HD8120_offline_Dialog),
    ymodemFileTransmit(new YmodemFileTransmit)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setWindowTitle("Assistant GNSS offline");
    ui->ftp_client->setFTP_addr("agnss.allystar.com");
    ui->ftp_client->setFTP_info("quectel","Agnss22@qt957");
    ui->dateTimeEdit->setDisplayFormat("yyyy-MM-dd HH:mm:ss");
    ui->progressBar_download->setValue(0);

    connect(ymodemFileTransmit, SIGNAL(transmitProgress(int)), this, SLOT(transmitProgress(int)));
    connect(ymodemFileTransmit, SIGNAL(transmitStatus(YmodemFileTransmit::Status)), this, SLOT(transmitStatus(YmodemFileTransmit::Status)));
}


HD8120_offline_Dialog::~HD8120_offline_Dialog()
{
    delete ui;
    delete ymodemFileTransmit;
}

void HD8120_offline_Dialog::transmitProgress(int progress)
{
    ui->progressBar_download->setValue(progress);
}

void HD8120_offline_Dialog::on_checkBox_useSysTime_stateChanged(int arg1)
{
    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTimeUtc());
    ui->dateTimeEdit->setEnabled(!arg1);
}

void HD8120_offline_Dialog::on_checkBox_useCurPos_stateChanged(int arg1)
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

void HD8120_offline_Dialog::on_pushButton_selectFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Select File!","./AGNSS_File",tr("EPH (*.rtcm)"));
    ui->lineEdit_filePath->setText(fileName);
}

void HD8120_offline_Dialog::on_pushButton_send_clicked()
{
    try {
        ui->pushButton_send->setEnabled(false);
        mainForm->sendResetCMD(ui->comboBox_restartMode->currentIndex());
        if(ui->groupBox_utc->isChecked()){
            //send utc
            sendUTC();
        }
        if(ui->groupBox_postion->isChecked()){
            //send postion
            sendPostion();
        }
    } catch (QString msg) {
        QMessageBox::warning(this, "Failed", msg, QMessageBox::Cancel);
        qDebug()<<msg;
    }
    ui->pushButton_send->setEnabled(true);
}


void HD8120_offline_Dialog::sendUTC()
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

void HD8120_offline_Dialog::sendPostion()
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

bool HD8120_offline_Dialog::createEPH_File(QString fileName,QString &newFile)
{
    if(fileName.isEmpty()){
        QMessageBox::warning(this,  "failed",  "File name is empty!",  "Close");
        return false;
    }

    QFileInfo fileInfo = QFileInfo(fileName);
    if(fileInfo.fileName().contains("GPS")){
        newFile = fileInfo.absolutePath() + "/LTEPHGPS";
        QFile::copy(fileName, newFile);
    }else if(fileInfo.fileName().contains("BDS")){
        newFile = fileInfo.absolutePath() + "/LTEPHBDS";
        QFile::copy(fileName, newFile);
    }
    else{
        newFile = fileName;
    }
    return true;
}

void HD8120_offline_Dialog::on_pushButton_Download_clicked()
{
    QString fileName;
    if(!createEPH_File(ui->lineEdit_filePath->text(),fileName))
        return;
    if(transmitButtonStatus == false)
    {
        disconnect(mainForm->m_serial, &QSerialPort::readyRead, mainForm, &MainWindow::ReadDataFromSerial);
//        mainForm->m_serial->write(QByteArray().fromHex("F1 D9 06 09 08 00 02 00 00 00 FF FF FF FF 15 01"));
//        mainForm->m_serial->waitForBytesWritten();
//        qgnss_sleep(2000);
//        mainForm->m_serial->write(QByteArray().fromHex("F1 D9 02 41 00 00 43 CB"));
//        mainForm->m_serial->waitForBytesWritten();
//        qgnss_sleep(500);
//        qDebug()<<mainForm->m_serial->readAll();
        mainForm->m_serial->close();

        ymodemFileTransmit->setFileName(fileName);
        ymodemFileTransmit->setPortName(mainForm->m_serial->portName());
        ymodemFileTransmit->setPortBaudRate(mainForm->m_serial->baudRate());
        if(ymodemFileTransmit->startTransmit() == true)
        {
            transmitButtonStatus = true;
            ui->pushButton_selectFile->setDisabled(true);
            ui->pushButton_Download->setText( "Cancel");
            ui->progressBar_download->setValue(0);
        }
        else
        {
            QMessageBox::warning(this,  "failed",  "File sent failed!",  "Close");
            connect(mainForm->m_serial, &QSerialPort::readyRead, mainForm, &MainWindow::ReadDataFromSerial);
            mainForm->m_serial->open(QIODevice::ReadWrite);
        }
    }
    else
    {
        ymodemFileTransmit->stopTransmit();
    }
}

void HD8120_offline_Dialog::transmitStatus(Ymodem::Status status)
{
    switch(status)
    {
        case YmodemFileTransmit::StatusEstablish:
        {
            break;
        }

        case YmodemFileTransmit::StatusTransmit:
        {
            break;
        }

        case YmodemFileTransmit::StatusFinish:
        {
            transmitButtonStatus = false;

            ui->pushButton_selectFile->setEnabled(true);
            ui->pushButton_Download->setText("Download");

            QMessageBox::warning(this,  "Success",  "The file was sent successfully!",  "Close");
            connect(mainForm->m_serial, &QSerialPort::readyRead, mainForm, &MainWindow::ReadDataFromSerial);
            mainForm->m_serial->open(QIODevice::ReadWrite);
            break;
        }

        case YmodemFileTransmit::StatusAbort:
        {
            transmitButtonStatus = false;

            ui->pushButton_selectFile->setEnabled(true);
            ui->pushButton_Download->setText("Download");

            QMessageBox::warning(this,  "failed",  " File sent failed!",  "Close");
            connect(mainForm->m_serial, &QSerialPort::readyRead, mainForm, &MainWindow::ReadDataFromSerial);
            mainForm->m_serial->open(QIODevice::ReadWrite);
            break;
        }

        case YmodemFileTransmit::StatusTimeout:
        {
            transmitButtonStatus = false;

            ui->pushButton_selectFile->setEnabled(true);
            ui->pushButton_Download->setText("Download");

            QMessageBox::warning(this,  "failed",  " File sent failed!",  "Close");
            connect(mainForm->m_serial, &QSerialPort::readyRead, mainForm, &MainWindow::ReadDataFromSerial);
            mainForm->m_serial->open(QIODevice::ReadWrite);
            break;
        }

        default:
        {
            transmitButtonStatus = false;
            ui->pushButton_selectFile->setEnabled(true);
            ui->pushButton_Download->setText("Download");
            QMessageBox::warning(this,  "failed",  " File sent failed!",  "Close");
            connect(mainForm->m_serial, &QSerialPort::readyRead, mainForm, &MainWindow::ReadDataFromSerial);
            mainForm->m_serial->open(QIODevice::ReadWrite);
        }
    }
}

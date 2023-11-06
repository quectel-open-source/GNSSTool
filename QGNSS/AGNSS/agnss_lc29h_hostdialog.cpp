#include "agnss_lc29h_hostdialog.h"
#include "ui_agnss_lc29h_hostdialog.h"
#include "mainwindow.h"
#include "CommonEPO_AG3335.h"
#include "common.h"

AGNSS_LC29H_hostDialog::AGNSS_LC29H_hostDialog(QWidget *parent) : QDialog(parent),
                                                                  ui(new Ui::AGNSS_LC29H_hostDialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Host EPO download");
    manager.setHost("agnss.queclocator.com");
    manager.setPort(21);
    manager.setUserName("QEPO");
    manager.setPassword("123456");
}

AGNSS_LC29H_hostDialog::~AGNSS_LC29H_hostDialog()
{
    delete ui;
}

bool AGNSS_LC29H_hostDialog::getEPO_6h_File(FTP_Manager &manager, QPlainTextEdit *msgWindow)
{
    QNetworkReply *reply = manager.get("/QGPS.DAT", QEPO_GPS_FILE_NAME);
    if (msgWindow)
        msgWindow->appendPlainText("Download QGPS.DAT ...");
    while (reply->isRunning())
    {
        qgnss_sleep(10);
    }
    if (reply->error())
    {
        if (msgWindow)
            msgWindow->appendPlainText(reply->errorString());
        return reply->error();
    }
    if (msgWindow)
        msgWindow->appendPlainText("Download success !");

    if (msgWindow)
        msgWindow->appendPlainText("Download QG_R.DAT ...");
    reply = manager.get("/QG_R.DAT", QEPO_GLONASS_FILE_NAME);
    while (reply->isRunning())
    {
        qgnss_sleep(10);
    }
    if (reply->error())
    {
        if (msgWindow)
            msgWindow->appendPlainText(reply->errorString());
        return reply->error();
    }
    if (msgWindow)
        msgWindow->appendPlainText("Download success !");

    if (msgWindow)
        msgWindow->appendPlainText("Download QBD2.DAT ...");
    reply = manager.get("/QBD2.DAT", QEPO_BDS_FILE_NAME);
    while (reply->isRunning())
    {
        qgnss_sleep(10);
    }
    if (reply->error())
    {
        if (msgWindow)
            msgWindow->appendPlainText(reply->errorString());
        return reply->error();
    }
    if (msgWindow)
        msgWindow->appendPlainText("Download success !");

    if (msgWindow)
        msgWindow->appendPlainText("Download QGA.DAT ...");
    reply = manager.get("/QGA.DAT", QEPO_GAL_FILE_NAME);
    while (reply->isRunning())
    {
        qgnss_sleep(10);
    }
    if (reply->error())
    {
        if (msgWindow)
            msgWindow->appendPlainText(reply->errorString());
        return reply->error();
    }
    if (msgWindow)
        msgWindow->appendPlainText("Download success !");

    return reply->error();
}

// false: success
bool AGNSS_LC29H_hostDialog::createQEPO_File(char type, QString fileName)
{
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly) || file.size() == 0)
    {
        qDebug() << "Three days of EPO file open error";
        return true;
    }
    if (type == 'C' || type == 'E')
        file.read(72);

    int32_t wn;
    double tow;
    QDateTime time = QDateTime::currentDateTime().toUTC();
    utc_to_gpstime(time.date().year(),
                   time.date().month(),
                   time.date().day(),
                   time.time().hour(),
                   time.time().minute(),
                   time.time().second(), &wn, &tow);
    int cur_gps_hour = (wn * 604800 + tow) / 3600;
    uint32_t epo_gps_hour;
    file.read((char *)&epo_gps_hour, 4);
    epo_gps_hour = epo_gps_hour & 0x00FFFFFF;
    int sgt = (cur_gps_hour - epo_gps_hour) / 6;
    QFile qEPO(QString("./AGNSS_File/Q%1.DAT").arg(type));
    if (!qEPO.open(QIODevice::WriteOnly))
    {
        qDebug() << "createQEPO_File:QEPO open error." << qEPO.errorString();
        return true;
    }
    if (type == 'G')
    {
        file.seek(sgt * EPO_DEMO_RECORD_SIZE * EPO_DEMO_MAX_GPS_SV);
        qEPO.write(file.read(EPO_DEMO_RECORD_SIZE * EPO_DEMO_MAX_GPS_SV));
        qEPO.close();
        qDebug() << "Create a file " << qEPO.fileName();
        return false;
    }
    if (type == 'C')
    {
        file.seek(sgt * EPO_DEMO_RECORD_SIZE * EPO_DEMO_MAX_BD_SV + 72);
        qEPO.write(file.read(EPO_DEMO_RECORD_SIZE * EPO_DEMO_MAX_BD_SV));
        qEPO.close();
        qDebug() << "Create a file " << qEPO.fileName();
        return false;
    }
    if (type == 'E')
    {
        file.seek(sgt * EPO_DEMO_RECORD_SIZE * EPO_DEMO_MAX_GAL_SV + 72);
        qEPO.write(file.read(EPO_DEMO_RECORD_SIZE * EPO_DEMO_MAX_GAL_SV));
        qEPO.close();
        qDebug() << "Create a file " << qEPO.fileName();
        return false;
    }
    if (type == 'R')
    {
        file.seek(sgt * EPO_DEMO_RECORD_SIZE * (EPO_DEMO_MAX_GPS_SV + EPO_DEMO_MAX_GLONASS_SV) + EPO_DEMO_RECORD_SIZE * EPO_DEMO_MAX_GPS_SV);
        qEPO.write(file.read(EPO_DEMO_RECORD_SIZE * EPO_DEMO_MAX_GLONASS_SV));
        qEPO.close();
        qDebug() << "Create a file " << qEPO.fileName();
        return false;
    }
    return true;
}

QString AGNSS_LC29H_hostDialog::sendUTC(QDateTime utc)
{
    QString data;
    data.sprintf("$PAIR590,%04d,%02d,%02d,%02d,%02d,%02d",
                 utc.date().year(),
                 utc.date().month(),
                 utc.date().day(),
                 utc.time().hour(),
                 utc.time().minute(),
                 utc.time().second());
    QByteArray cmd = data.toLatin1();
    addNMEA_CS(cmd);
    mainForm->sendData(cmd);
    qgnss_sleep(10);
    return "";
}

QString AGNSS_LC29H_hostDialog::sendPosition(double lat, double lon, double height)
{
    QString data;
    data.sprintf("$PAIR600,%.6f,%.6f,%.2f,50.0,50.0,0.0,100.0",
                 lat,
                 lon,
                 height);
    QByteArray cmd = data.toLatin1();
    addNMEA_CS(cmd);
    mainForm->sendData(cmd);
    qgnss_sleep(10);
    return "";
}

QString AGNSS_LC29H_hostDialog::sendHostEPO(QString fileName, uint8_t type)
{
    int32_t wn;
    double tow;
    QDateTime time = QDateTime::currentDateTime().toUTC();
    utc_to_gpstime(time.date().year(),
                   time.date().month(),
                   time.date().day(),
                   time.time().hour(),
                   time.time().minute(),
                   time.time().second(), &wn, &tow);
    int cur_gps_hour = (wn * 604800 + tow) / 3600;
    qInfo() << "Current GPS hour:" << cur_gps_hour;

    QFile EPO_File(fileName);
    if (!EPO_File.open(QIODevice::ReadOnly))
    {
        return fileName + EPO_File.errorString();
    }

    char temp_buffer[512] = {0};
    uint8_t data_buffer[EPO_DEMO_RECORD_SIZE] = {0};
    int32_t sv_prn = 0;
    if (type == EPO_DEMO_MODE_BEIDOU || type == EPO_DEMO_MODE_GALILEO)
    {
        EPO_File.read(72);
    }
    if (type == EPO_DEMO_MODE_GLONASS)
    {
        EPO_File.read(72 * 32);
    }
    while (!EPO_File.atEnd())
    {
        unsigned int *epobuf = (unsigned int *)data_buffer;
        EPO_File.read((char *)data_buffer, 72);
        sv_prn = epo_demo_get_sv_prn(type, data_buffer);
        qInfo() << "EPO GPS hour:" << (data_buffer[2] * 0x10000 + data_buffer[1] * 0x100 + data_buffer[0])
                << " SVID:" << data_buffer[3]
                << " PRN:" << sv_prn;
        sprintf((char *)temp_buffer,
                "$PAIR471,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X,%X",
                (unsigned int)type,
                (unsigned int)sv_prn,
                epobuf[0], epobuf[1], epobuf[2], epobuf[3], epobuf[4], epobuf[5],
                epobuf[6], epobuf[7], epobuf[8], epobuf[9], epobuf[10], epobuf[11],
                epobuf[12], epobuf[13], epobuf[14], epobuf[15], epobuf[16], epobuf[17]);
        QByteArray cmd(temp_buffer);
        addNMEA_CS(cmd);
        mainForm->sendData(cmd);
        qgnss_sleep(20);
        memset(temp_buffer, 0, sizeof(temp_buffer));
    }
    return "";
}

void AGNSS_LC29H_hostDialog::on_checkBox_useCurPos_stateChanged(int arg1)
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

void AGNSS_LC29H_hostDialog::on_checkBox_useCurTime_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    QDateTime UTC(QDateTime::currentDateTime().toUTC()); // current UTC time
    ui->lineEdit_UTC->setText(UTC.toString(UTC_DATE_FORMAT));
}

void AGNSS_LC29H_hostDialog::on_pushButton_transfer_clicked()
{
    ui->pushButton_transfer->setEnabled(false);
    QString msg = transfer_AGNSS_Data();
    if (msg != "")
    {
        QMessageBox::warning(this, "Warning", msg, QMessageBox::Ok);
    }
    ui->pushButton_transfer->setEnabled(true);
}

QString AGNSS_LC29H_hostDialog::transfer_AGNSS_Data()
{
    // get EPO 3day File from FTP
    if (getEPO_6h_File(manager, ui->plainTextEdit))
    {
        return "Download EPO file error.";
    }

    // Create current time segment EPO file
    //    if (
    //            createQEPO_File('G', EPO_GPS_ONLY_FILE_NAME) ||
    //            createQEPO_File('C', EPO_BD_ONLY_FILE_NAME) ||
    //            createQEPO_File('E', "./AGNSS_File/QGA.DAT") ||
    //        createQEPO_File('R', EPO_GR_ONLY_FILE_NAME)
    //            )
    //    {
    //        return "Error creating current time segment EPO file.";
    //    }

    // send restart cmd
    if (ui->comboBox_restartMode->currentIndex())
    {
        mainForm->sendResetCMD(ui->comboBox_restartMode->currentIndex());
        qgnss_sleep(600);
    }

    // send UTC
    if (ui->groupBox_time->isChecked())
    {
        ui->plainTextEdit->appendPlainText("send UTC.");
        if (ui->checkBox_useCurTime->isChecked())
        {
            on_checkBox_useCurTime_stateChanged(true);
        }
        sendUTC(QDateTime::fromString(ui->lineEdit_UTC->text(), UTC_DATE_FORMAT));
    }

    // send position
    if (ui->groupBox_position->isChecked())
    {
        ui->plainTextEdit->appendPlainText("send position.");
        sendPosition(ui->lineEdit_lat->text().toDouble(),
                     ui->lineEdit_long->text().toDouble(),
                     ui->lineEdit_alt->text().toDouble());
    }

    // send host EPO
    ui->plainTextEdit->appendPlainText("send host EPO......");
    QString msg;
    if (ui->checkBox_G->isChecked())
    {
        msg = sendHostEPO(QEPO_GPS_FILE_NAME, EPO_DEMO_MODE_GPS);
        if (msg != "")
        {
            return msg;
        }
    }
    if (ui->checkBox_R->isChecked())
    {
        msg = sendHostEPO(QEPO_GLONASS_FILE_NAME, EPO_DEMO_MODE_GLONASS);
        if (msg != "")
        {
            return msg;
        }
    }
    if (ui->checkBox_E->isChecked())
    {
        msg = sendHostEPO(QEPO_GAL_FILE_NAME, EPO_DEMO_MODE_GALILEO);
        if (msg != "")
        {
            return msg;
        }
    }
    if (ui->checkBox_C->isChecked())
    {
        msg = sendHostEPO(QEPO_BDS_FILE_NAME, EPO_DEMO_MODE_BEIDOU);
        if (msg != "")
        {
            return msg;
        }
    }
    ui->plainTextEdit->appendPlainText("*******Success*******");
    return "";
}

// void AGNSS_LC29H_hostDialog::on_pushButton_HEPO_clicked()
//{
//     QString fileName = QFileDialog::getOpenFileName(this, tr("Select File to Replay"), "./AGNSS_File", tr("All File(*)"));
//     QFile epoFile(fileName);
//     epoFile.open(QIODevice::ReadOnly);
//     uint32_t epo_gps_hour;
//     while(epoFile.read((char *)&epo_gps_hour, 4)){
//         epoFile.read(68);
//         epo_gps_hour = epo_gps_hour & 0x00FFFFFF;
//         qDebug()<<"epo_gps_hour: "<<epo_gps_hour;
//     }
// }

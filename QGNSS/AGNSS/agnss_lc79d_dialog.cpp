#include "agnss_lc79d_dialog.h"
#include "ui_agnss_lc79d_dialog.h"
#include "QFileDialog"
#include "mainwindow.h"

AGNSS_LC79D_Dialog::AGNSS_LC79D_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AGNSS_LC79D_Dialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("AGNSS Dialog");
    ui->ftp_client->setFTP_info("QLTO","123456");
    ui->progressBar_download->setValue(0);
}

AGNSS_LC79D_Dialog::~AGNSS_LC79D_Dialog()
{
    delete ui;
}

void AGNSS_LC79D_Dialog::on_pushButton_transfer_clicked()
{
    ui->pushButton_transfer->setEnabled(false);
    if(ui->comboBox_restartMode->currentIndex()){
        mainForm->sendResetCMD(ui->comboBox_restartMode->currentIndex());
        qgnss_sleep(500);
    }
    mainForm->sendData("$PQSTOPGNSS*10\r\n");
    qgnss_sleep(500);
//    if(ui->checkBox_sendFile->isChecked())
//        sendAGNSS_File();
    if(ui->checkBox_useCurTime->isChecked())
        on_checkBox_useCurTime_stateChanged(0);
    if(ui->checkBox_time->isChecked())
        sendTime();
    if(ui->checkBox_pos->isChecked())
        sendPosition();
    mainForm->sendData("$PQSTARTGNSS*48\r\n");
    ui->pushButton_transfer->setEnabled(true);
}

void AGNSS_LC79D_Dialog::on_pushButton_selectFile_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,"Select File!","./AGNSS_File");
    ui->lineEdit_filePath->setText(fileName);
}

void AGNSS_LC79D_Dialog::downloadLTO_DataToModule()
{
    try {
        QFile LTO_File(ui->lineEdit_filePath->text());
        if(!LTO_File.open(QIODevice::ReadOnly))
            throw LTO_File.errorString();
        mainForm->sendData("$PQLTODOWNLOAD\r\n");
        ui->progressBar_download->setValue(1);
        qgnss_sleep(5000);
        QByteArray FileInfo = QByteArray::fromHex("AA 01 01 08");
        uint32_t fileSize  = LTO_File.size();
        if(!intInRange(1, LTO_FILE_MAX_SIZE, fileSize))
            throw "Error in the LTO file size.";
        char littleEndian[4];
        littleEndian[0] = fileSize & 0x000000ff;
        littleEndian[1] = (fileSize >> 8) & 0x000000ff;
        littleEndian[2] = (fileSize >> 16) & 0x000000ff;
        littleEndian[3] = (fileSize >> 24) & 0x000000ff;
        FileInfo.append(littleEndian, 4);
        QByteArray crc32Field(littleEndian,4);
        crc32Field.append(LTO_File.readAll());
        ui->label_FileSize->setText(QString("FileSize: 0x%1").arg(LTO_File.size(),8,16, QLatin1Char('0')));
        qInfo()<<"LTO file size:"<<LTO_File.size()<<QString("(0x%1)").arg(LTO_File.size(),8,16);
        qInfo()<<"CRC32 Field size:"<<crc32Field.size();
        uint32_t crc32 = c_crc32((uint8_t *)crc32Field.constData(), crc32Field.size());
        ui->label_CRC->setText(QString("  CRC: 0x%1").arg(crc32,8,16, QLatin1Char('0')));
        littleEndian[0] = crc32 & 0x000000ff;
        littleEndian[1] = (crc32 >> 8) & 0x000000ff;
        littleEndian[2] = (crc32 >> 16) & 0x000000ff;
        littleEndian[3] = (crc32 >> 24) & 0x000000ff;
        FileInfo.append(littleEndian, 4);
        //AA |*01 01 08 4D A4 01 00 4E 6E 07 E6*| 21 55
        //   |--------------XOR-----------------|     this is true
        add_CS(FileInfo, 1);
        FileInfo.append(0x55);
        qDebug()<<"Send LTO File Information to Module: "<<FileInfo.toHex(' ').toUpper();
        mainForm->sendData(FileInfo);
        qgnss_sleep(6000);
        LTO_File.seek(0);
        QByteArray LTO_Data = LTO_File.read(1024 * 16);
        while(LTO_Data.size()){
            qDebug()<<"LTO_Data size:"<<LTO_Data.size();
            mainForm->sendData(LTO_Data);
            qgnss_sleep(5000);
//            disconnect(mainForm->m_serial, &QSerialPort::readyRead, mainForm, &MainWindow::readData);
//            connect(mainForm->m_serial, &QSerialPort::readyRead, mainForm, &MainWindow::readData);
            LTO_Data.clear();
            LTO_Data = LTO_File.read(1024 * 16);
            ui->progressBar_download->setValue(LTO_File.pos()*100.0/LTO_File.size());
        }
        ui->progressBar_download->setValue(100);
    } catch (QString msg) {
        ui->progressBar_download->setValue(0);
        QMessageBox::warning(this, "Failed", msg, QMessageBox::Cancel);
        qDebug()<<msg;
    } catch (...) {
        QMessageBox::warning(this, "Failed", "Unknown error", QMessageBox::Cancel);
    }
}

void AGNSS_LC79D_Dialog::sendTime()
{
    QList<QString> time,date;
    date = ui->lineEdit_UTC->text().split(' ');
    if(date.size () > 1)
    {
        //Extract time first,
        time = date[1].split(':');
        date = date[0].split('-');
    }
    if(date.size()>2 && time.size()>2)
    {

        //TIME aiding $PQSETASSTIME,<year>,<month>,<day>,<hour>,<min>,<sec>*<checksum><CR><LF>
        QString cmd;
        cmd.sprintf("$PQSETASSTIME,%d,%d,%d,%d,%d,%d",
                    date[0].toInt(),
                    date[1].toInt(),
                    date[2].toInt(),
                    time[0].toInt(),
                    time[1].toInt(),
                    time[2].toInt()
                    );
        addNMEA_CS(cmd);
        mainForm->sendData(cmd.toLatin1());
        qgnss_sleep(500);
    }
}

void AGNSS_LC79D_Dialog::sendPosition()
{
    //POS aiding  $PQSETASSPOS,<lat>,<lon>,<alt>[,<hor_acc>,<ver_acc>]*<checksum><CR><LF>
    QString cmd;
    cmd.sprintf("$PQSETASSPOS,%.6f,%.6f,%.2f",
                ui->lineEdit_lat->text().toDouble(),
                ui->lineEdit_long->text().toDouble(),
                ui->lineEdit_alt->text().toDouble()
                );
    addNMEA_CS(cmd);
    mainForm->sendData(cmd.toLatin1());
    qgnss_sleep(500);
}

void AGNSS_LC79D_Dialog::on_pushButton_download_clicked()
{
    ui->pushButton_download->setEnabled(false);
    downloadLTO_DataToModule();
    ui->pushButton_download->setEnabled(true);
}

void AGNSS_LC79D_Dialog::on_checkBox_useCurPos_stateChanged(int arg1)
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

void AGNSS_LC79D_Dialog::on_checkBox_useCurTime_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    QDateTime UTC(QDateTime::currentDateTime().toUTC());//current UTC time
    ui->lineEdit_UTC->setText(UTC.toString("yyyy-MM-dd hh:mm:ss"));

}

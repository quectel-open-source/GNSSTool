#include "configuration_l76k.h"
#include "ui_configuration_l76k.h"
#include "mainwindow.h"
#include "common.h"

configuration_L76K::configuration_L76K(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configuration_L76K),
    statusBar(new QStatusBar)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    ui->gridLayout_13->addWidget(statusBar);
    statusBar->showMessage("No message");
    setWindowFlags(Qt::WindowCloseButtonHint | Qt::MSWindowsFixedSizeDialogHint);
    setWindowTitle("Configuration L76K");
}

configuration_L76K::~configuration_L76K()
{
    delete ui;
    delete statusBar;
}

void configuration_L76K::on_pushButton_send_clicked()
{    
    mainForm->m_serial->write((ui->lineEdit_cmd->text()+"\r\n").toLatin1());
}

void configuration_L76K::on_pushButton_PCAS_1_clicked()
{
    int baudID = ui->comboBox_baud->currentIndex();
    QByteArray cmd = QString("$PCAS01,%1").arg(baudID).toLatin1();
    addNMEA_MC_CS(cmd);
    ui->lineEdit_cmd->setText(cmd);
}

void configuration_L76K::on_pushButton_PCAS_2_clicked()
{
    QByteArray cmd = QString("$PCAS02,%1").arg(ui->comboBox_FixInterval->currentText()).toLatin1();
    addNMEA_MC_CS(cmd);
    ui->lineEdit_cmd->setText(cmd);
}

void configuration_L76K::on_pushButton_PCAS_3_clicked()
{
    QByteArray cmd = QString("$PCAS03,%1,%2,%3,%4,%5,%6,%7,%8,0,0,,,0,0")
            .arg(ui->lineEdit_gga->text())
            .arg(ui->lineEdit_gll->text())
            .arg(ui->lineEdit_gsa->text())
            .arg(ui->lineEdit_gsv->text())
            .arg(ui->lineEdit_rmc->text())
            .arg(ui->lineEdit_vtg->text())
            .arg(ui->lineEdit_zda->text())
            .arg(ui->lineEdit_ant->text())
            .toLatin1();
    addNMEA_MC_CS(cmd);
    ui->lineEdit_cmd->setText(cmd);
}

void configuration_L76K::on_pushButton_PCAS_4_clicked()
{
    QByteArray cmd = QString("$PCAS04,%1").arg(ui->comboBox_gnssMode->currentIndex()+1).toLatin1();
    addNMEA_MC_CS(cmd);
    ui->lineEdit_cmd->setText(cmd);
}

void configuration_L76K::on_pushButton_PCAS_10_clicked()
{
    QByteArray cmd = QString("$PCAS10,%1").arg(ui->comboBox_restartType->currentIndex()).toLatin1();
    addNMEA_MC_CS(cmd);
    ui->lineEdit_cmd->setText(cmd);
}

void configuration_L76K::on_pushButton_CFG_PRT_clicked()
{
    struct
    {
      uint8_t PortID:8;
      uint8_t ProtoMask0:1;
      uint8_t ProtoMask1:1;
      uint8_t :2;
      uint8_t ProtoMask2:1;
      uint8_t ProtoMask3:1;
      uint8_t :2;
      uint16_t :6;
      uint16_t dataBit:2;
      uint16_t :1;
      uint16_t praity:3;
      uint16_t stopBit:2;
      uint16_t :2;
      uint32_t baud:32;
    } sCFG_PRT;
    memset(&sCFG_PRT,0,sizeof (sCFG_PRT));
    QByteArray cmd;
    cmd.append(0xBA);
    cmd.append(0xCE);
    cmd.append(0x08);
    cmd.append(static_cast<char>(0));
    cmd.append(0x06);
    cmd.append(static_cast<char>(0));
    sCFG_PRT.PortID = ui->lineEdit_portID->text().toUInt();
    sCFG_PRT.ProtoMask0 = ui->checkBox_BinInput->isChecked();
    sCFG_PRT.ProtoMask1 = ui->checkBox_TextInput->isChecked();
    sCFG_PRT.ProtoMask2 = ui->checkBox_BinOut->isChecked();
    sCFG_PRT.ProtoMask3 = ui->checkBox_TextOut->isChecked();
    sCFG_PRT.dataBit = ui->comboBox_dataBit->currentIndex();
    sCFG_PRT.praity = ui->comboBox_praity->currentIndex() == 2?4:ui->comboBox_praity->currentIndex();
    sCFG_PRT.stopBit = ui->comboBox_stopBit->currentIndex();
    sCFG_PRT.baud = ui->lineEdit_Baudrate->text().toUInt();
    cmd.append((char *)&sCFG_PRT,sizeof(sCFG_PRT));
    addChecksum(cmd);
    ui->lineEdit_cmd->setText(cmd.toHex(' ').toUpper());
}

void configuration_L76K::on_pushButton_CFG_PRT_poll_clicked()
{
    mainForm->m_serial->write(QByteArray::fromHex("BA CE 00 00 06 01 00 00 06 01"));
    statusBar->showMessage("Poll CFG-PRT");
}

void configuration_L76K::addChecksum(QByteArray &cmd)
{
    uint32_t Checksum,payload = 0;
    uint16_t len = (static_cast<uint8_t>(cmd.at(3)) << 8) + static_cast<uint8_t>(cmd.at(2));
    Checksum = (static_cast<uint8_t>(cmd.at(5)) << 24) + (static_cast<uint8_t>(cmd.at(4)) << 16) + len;
    for (int i = 0; i < (len / 4); i++)
    {
//        payload = static_cast<uint8_t>(cmd.at(9+i*4))<<24;
//        payload = (static_cast<uint8_t>(cmd.at(8+i*4))<<16) + payload;
//        payload = (static_cast<uint8_t>(cmd.at(7+i*4))<<8) + payload;
//        payload = static_cast<uint8_t>(cmd.at(6+i*4)) + payload;
//        Checksum = Checksum + payload;
        Checksum = Checksum + *((uint32_t *)(&(cmd.data()[6+i*4])));
    }
//    cmd.append(Checksum&0xff);
//    cmd.append(Checksum>>8&0xff);
//    cmd.append(Checksum>>16&0xff);
//    cmd.append(Checksum>>24);
    cmd.append(((uint8_t *)(&Checksum))[0]);
    cmd.append(((uint8_t *)(&Checksum))[1]);
    cmd.append(((uint8_t *)(&Checksum))[2]);
    cmd.append(((uint8_t *)(&Checksum))[3]);
//    cmd.append(0x0D);
//    cmd.append(0x0A);
}

void configuration_L76K::on_pushButton_CFG_MSG_clicked()
{
    QByteArray cmd;
    cmd.append(0xBA);
    cmd.append(0xCE);
    cmd.append(0x04);
    cmd.append(static_cast<char>(0));
    cmd.append(0x06);
    cmd.append(0x01);
    cmd.append(ui->lineEdit_cfg_msg_class->text().toUInt());
    cmd.append(ui->lineEdit_cfg_msg_id->text().toUInt());
    cmd.append(ui->lineEdit_cfg_msg_rate->text().toUInt()&0xff);
    cmd.append(ui->lineEdit_cfg_msg_rate->text().toUInt()>>8);
    addChecksum(cmd);
    ui->lineEdit_cmd->setText(cmd.toHex(' ').toUpper());
}

void configuration_L76K::on_pushButton_CFG_MSG_poll_clicked()
{
    mainForm->m_serial->write(QByteArray::fromHex("BA CE 00 00 06 01 00 00 06 01"));
    statusBar->showMessage("Poll CFG-MSG");
}

void configuration_L76K::on_pushButton_CFG_RST_clicked()
{
    struct
    {
      uint16_t B0:1;
      uint16_t B1:1;
      uint16_t B2:1;
      uint16_t B3:1;
      uint16_t B4:1;
      uint16_t B5:1;
      uint16_t B6:1;
      uint16_t B7:1;
      uint16_t B8:1;
      uint16_t B9:1;
      uint16_t :6;
      uint8_t ResetMode:8;
      uint8_t StartMode:8;
    } sCFG_RST;
    memset(&sCFG_RST,0,sizeof (sCFG_RST));
    QByteArray cmd;
    cmd.append(0xBA);
    cmd.append(0xCE);
    cmd.append(0x04);
    cmd.append(static_cast<char>(0));
    cmd.append(0x06);
    cmd.append(0x02);
    sCFG_RST.B0 = ui->checkBox_clear_0EPH->isChecked();
    sCFG_RST.B1 = ui->checkBox_clear_1Almanac->isChecked();
    sCFG_RST.B2 = ui->checkBox_clear_2HealthInfo->isChecked();
    sCFG_RST.B3 = ui->checkBox_clear_3Ionospheric->isChecked();
    sCFG_RST.B4 = ui->checkBox_clear_4LocationInfo->isChecked();
    sCFG_RST.B5 = ui->checkBox_clear_5ClockDeviation->isChecked();
    sCFG_RST.B6 = ui->checkBox_clear_6CrystalParameters->isChecked();
    sCFG_RST.B7 = ui->checkBox_clear_7UTCCorrection->isChecked();
    sCFG_RST.B8 = ui->checkBox_clear_8RTC->isChecked();
    sCFG_RST.B9 = ui->checkBox_clear_9ConfigurationInfo->isChecked();
    sCFG_RST.ResetMode = ui->comboBox_ResetMode->currentIndex() == 3?4:ui->comboBox_ResetMode->currentIndex();
    sCFG_RST.StartMode = ui->comboBox_StartMode->currentIndex();
    cmd.append((char *)&sCFG_RST,sizeof(sCFG_RST));
    addChecksum(cmd);
    ui->lineEdit_cmd->setText(cmd.toHex(' ').toUpper());
}

void configuration_L76K::on_pushButton_CFG_RATE_clicked()
{
    QByteArray cmd = QByteArray::fromHex("BA CE 04 00 06 04");/*
    cmd.append(0xBA);
    cmd.append(0xCE);
    cmd.append(0x04);
    cmd.append(static_cast<char>(0));
    cmd.append(0x06);
    cmd.append(0x04);*/
    cmd.append(ui->lineEdit_cfg_rate_Interval->text().toUInt()&0xff);
    cmd.append(ui->lineEdit_cfg_rate_Interval->text().toUInt()>>8);
    cmd.append(static_cast<char>(0));
    cmd.append(static_cast<char>(0));
    addChecksum(cmd);
    ui->lineEdit_cmd->setText(cmd.toHex(' ').toUpper());
}

void configuration_L76K::on_pushButton_CFG_RATE_poll_clicked()
{
    mainForm->m_serial->write("BA CE 00 00 06 04 00 00 06 04");
    statusBar->showMessage("Poll CFG-RATE");
}

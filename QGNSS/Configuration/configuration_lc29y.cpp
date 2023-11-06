#include "configuration_lc29y.h"
#include "ui_configuration_lc29y.h"
#include "mainwindow.h"

configuration_LC29Y::configuration_LC29Y(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configuration_LC29Y),
    statusBar(new QStatusBar(this))

{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setWindowTitle("Configuration:LC29Y");
    statusBar->showMessage("No message");
    ui->verticalLayout->addWidget(statusBar);
}

configuration_LC29Y::~configuration_LC29Y()
{
    delete ui;
}

void configuration_LC29Y::addCheckSum(QByteArray &cmdload)
{
    uint8_t CHK1 = 0;
    uint8_t CHK2 = 0;
    for(int i = 2; i < cmdload.length(); i++)
    {
        CHK1 = CHK1 + (uint8_t)cmdload[i];
        CHK2 = CHK2 + CHK1;
        CHK1 &= 0xff;
        CHK2 &= 0xff;
    }
    cmdload.append(CHK1);
    cmdload.append(CHK2);
}

void configuration_LC29Y::on_pushButton_PRT_GC_clicked()
{
    QByteArray cmd = QByteArray::fromHex("F1 D9 06 00 08 00");
    cmd.append(ui->lineEdit_portID->text().toUInt());
    cmd.append(QByteArray::fromHex("00 00 00"));
    uint32_t rate = ui->lineEdit_baudrate->text().toUInt();
    cmd.append(rate);
    cmd.append(rate>>8);
    cmd.append(rate>>16);
    cmd.append(rate>>24);
    addCheckSum(cmd);
    ui->lineEdit_cmd->setText(cmd.toHex(' ').toUpper());
}

void configuration_LC29Y::on_pushButton_send_clicked()
{
    mainForm->m_serial->write(QByteArray::fromHex(ui->lineEdit_cmd->text().toLatin1()));
    showMSG(QString("The command has been sent."));
}

void configuration_LC29Y::on_pushButton_PRT_GC_2_clicked()
{
    QByteArray cmd = QByteArray::fromHex("F1 D9 06 01 03 00");
    cmd.append(ui->comboBox_groupAndNumber->currentText().split(' ')[0].toInt(NULL,16));
    cmd.append(ui->comboBox_groupAndNumber->currentText().split(' ')[1].toInt(NULL,16));
    cmd.append(ui->lineEdit_msg_rate->text().toUInt());
    addCheckSum(cmd);
    ui->lineEdit_cmd->setText(cmd.toHex(' ').toUpper());
}

void configuration_LC29Y::on_pushButton_PRT_GC_NAVSAT_clicked()
{
    QByteArray cmd = QByteArray::fromHex("F1 D9 06 0C 04 00");
    uint32_t mask = 0;
    mask = ui->checkBox_bdB1I->isChecked()?mask|BeiDou_B1I:mask;
    mask = ui->checkBox_bdB2a->isChecked()?mask|BeiDou_B2a:mask;
    mask = ui->checkBox_gloG1->isChecked()?mask|GLONASS_G1:mask;
    mask = ui->checkBox_gpsL1->isChecked()?mask|GPS_L1:mask;
    mask = ui->checkBox_gpsL5->isChecked()?mask|GPS_L5:mask;
    mask = ui->checkBox_galE5a->isChecked()?mask|Galileo_E5a:mask;
    mask = ui->checkBox_gpsL1C->isChecked()?mask|GPS_L1C:mask;
    mask = ui->checkBox_qzssL1->isChecked()?mask|QZSS_L1:mask;
    mask = ui->checkBox_qzssL5->isChecked()?mask|QZSS_L5:mask;
    mask = ui->checkBox_galileoE1->isChecked()?mask|Galileo_E1:mask;
    cmd.append(mask);
    cmd.append(mask>>8);
    cmd.append(mask>>16);
    cmd.append(mask>>24);
    addCheckSum(cmd);
    ui->lineEdit_cmd->setText(cmd.toHex(' ').toUpper());
}

void configuration_LC29Y::showMSG(QString msg)
{
    QDateTime current_time    = QDateTime::currentDateTime();
    msg = current_time.toString("[hh:mm:ss] ") + msg;
    statusBar->showMessage(msg);
}

void configuration_LC29Y::on_pushButton_PRT_GC_CFG_SIMPLERST_clicked()
{
    QByteArray cmd = QByteArray::fromHex("F1 D9 06 40 01 00");
    cmd.append(ui->comboBox_reset_mode->currentText().split(' ')[0].toUInt(NULL,16));
    addCheckSum(cmd);
    ui->lineEdit_cmd->setText(cmd.toHex(' ').toUpper());
}

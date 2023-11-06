#include "Configuration/configuration_lc29d.h"
#include "ui_configuration_lc29d.h"
#include "bream.h"
#include "bream_handler.h"
#include "common.h"
#include <QDebug>
#include <QMessageBox>
#include <QKeyEvent>

ConfigurationLC29D::ConfigurationLC29D(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConfigurationLC29D),
    statusBar(new QStatusBar(this))
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->treeWidget->setColumnWidth(0,400);
    this->setAttribute(Qt::WA_QuitOnClose,false);
    this->setWindowTitle("Configuration:LC29D");

    ui->treeWidget->hideColumn(1);
    ui->treeWidget->hideColumn(2);
    ui->gridLayout_2->addWidget(statusBar);
    statusBar->showMessage("No message");
    hasPoll<<13<<16<<18<<21<<22<<24<<25;
    hideIndex<<24<<25;
    QTreeWidgetItemIterator it(ui->treeWidget);
    while (*it) {
        if(hideIndex.contains((*it)->text(1).toInt()))
            (*it)->setHidden(true);
        ++it;
    }
//    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
}

ConfigurationLC29D::~ConfigurationLC29D()
{
    delete ui;
}

void ConfigurationLC29D::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    ui->stackedWidget->setCurrentWidget(ui->stackedWidget->findChildren<QWidget *>(current->text(2))[0]);
    ui->pushButton_poll->setEnabled(hasPoll.contains(ui->stackedWidget->currentIndex()));
}

void ConfigurationLC29D::on_treeWidget_itemDoubleClicked(QTreeWidgetItem *item, int column)
{
    qDebug()<<ui->stackedWidget->currentIndex();
    ui->stackedWidget->setCurrentIndex(ui->stackedWidget->currentIndex());
    QString str = "pushButton_GC"+QString::number(ui->stackedWidget->currentIndex());
    qDebug()<<str;
    QPushButton* pushButton = ui->stackedWidget->findChild<QPushButton*>("pushButton_GC"+QString::number(ui->stackedWidget->currentIndex()));
    if(!pushButton)
        return;
    qDebug()<<pushButton->text();
    pushButton->click();
    ui->pushButton_send->click();
}

void ConfigurationLC29D::on_pushButton_send_clicked()
{
    QByteArray text = QByteArray::fromHex(ui->lineEditCommand->text().toLocal8Bit());
    emit sendData(text.data(), text.size());
}

//BRM-ASC-SLEEP (0x02 0x41)
void ConfigurationLC29D::on_pushButton_GC0_clicked()
{
    LD2BRM_AscPwrCycleCommandPayload payload0;
    memset(&payload0,0,sizeof(payload0));
    char buf[1024] = {0};
    payload0.duration      = ui->lineEdit_duration->text().toUInt();
    payload0.flags         = 0x02;
    payload0.wakeupSources = ui->checkBox_wakeup_uartrx->isChecked()?0x08:0x00;
    payload0.wakeupSources = ui->checkBox_wakeup_extint0->isChecked() ? payload0.wakeupSources|0x10 : payload0.wakeupSources;
    payload0.wakeupSources = ui->checkBox_wakeup_spics->isChecked() ? payload0.wakeupSources|0x80 : payload0.wakeupSources;
    BuildBreamFrame(BRM_ASC_PMREQ, sizeof(LD2BRM_AscPwrCycleCommandPayload), (unsigned char*) &payload0, ( char*) &buf[0]);
    QByteArray lineText(buf,sizeof(LD2BRM_AscPwrCycleCommandPayload)+8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}

void ConfigurationLC29D::on_pushButton_GC4_clicked()
{
    LD2BRM_AstIniPosXyzInputPayload payload0;
    memset(&payload0,0,sizeof(payload0));
    char buf[1024] = {0};
    payload0.type         = 0x01;
    payload0.ecefX        = ui->lineEdit_lat->text().toDouble() * 1e7;
    payload0.ecefY        = ui->lineEdit_lon->text().toDouble() * 1e7;
    payload0.ecefZ        = ui->lineEdit_alt->text().toInt();
    payload0.posAcc       = ui->lineEdit_posAcc->text().toUInt();
    BuildBreamFrame(BRM_AST_INI, sizeof(LD2BRM_AstIniPosXyzInputPayload), (unsigned char*) &payload0, ( char*) &buf[0]);
    QByteArray lineText(buf,sizeof(LD2BRM_AstIniPosXyzInputPayload)+8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}

void ConfigurationLC29D::on_pushButton_GC5_clicked()
{
    LD2BRM_AstRefTimeUtcInputPayload payload0;
    memset(&payload0,0,sizeof(payload0));
    char buf[1024] = {0};
    payload0.type         = 0x10;
    payload0.leapSecs     = static_cast<char>(ui->lineEdit_leapSecs->text().toInt());
    payload0.year         = ui->lineEdit_year->text().toUShort();
    payload0.month        = static_cast<uint8_t>(ui->lineEdit_month->text().toUInt());
    payload0.day          = static_cast<uint8_t>(ui->lineEdit_day->text().toUInt());
    payload0.hour         = static_cast<uint8_t>(ui->lineEdit_hour->text().toUInt());
    payload0.minute       = static_cast<uint8_t>(ui->lineEdit_minute->text().toUInt());
    payload0.second       = static_cast<uint8_t>(ui->lineEdit_second->text().toUInt());
    payload0.ns           = ui->lineEdit_ns->text().toUInt();
    payload0.tAccS        = ui->lineEdit_tAccS->text().toUShort();
    payload0.tAccNs       = ui->lineEdit_tAccNs->text().toUInt();
    BuildBreamFrame(BRM_AST_INI,  sizeof(LD2BRM_AstRefTimeUtcInputPayload), (unsigned char*) &payload0, ( char*) &buf[0]);
    QByteArray lineText(buf,sizeof(LD2BRM_AstRefTimeUtcInputPayload)+8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}

void ConfigurationLC29D::on_pushButton_GC7_clicked()
{
    LD2BRM_PvtResetAccDistanceCommandPayload payload0;
    memset(&payload0,0,sizeof(payload0));
    char buf[1024] = {0};
    BuildBreamFrame(BRM_PVT_RESETODO,  0, (unsigned char*) &payload0, ( char*) &buf[0]);
    QByteArray lineText(buf,8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}

void ConfigurationLC29D::on_pushButton_GC12_clicked()
{
    LD2BRM_StpConfigCommandPayload payload0;
    memset(&payload0,0,sizeof(payload0));
    char buf[1024] = {0};
    payload0.clearMask  = ui->checkBox_CM_ioPort->isChecked()?payload0.clearMask|0x01:payload0.clearMask;
    payload0.clearMask  = ui->checkBox_CM_msgConf->isChecked()?payload0.clearMask|0x02:payload0.clearMask;
    payload0.clearMask  = ui->checkBox_CM_infMsg->isChecked()?payload0.clearMask|0x04:payload0.clearMask;
    payload0.clearMask  = ui->checkBox_CM_PVTConf->isChecked()?payload0.clearMask|0x08:payload0.clearMask;
    payload0.clearMask  = ui->checkBox_CM_ASConf->isChecked()?payload0.clearMask|0x10:payload0.clearMask;
    payload0.clearMask  = ui->checkBox_CM_ftsConf->isChecked()?payload0.clearMask|0x1000:payload0.clearMask;

    payload0.saveMask  = ui->checkBox_SM_ioPort->isChecked()?payload0.saveMask|0x01:payload0.saveMask;
    payload0.saveMask  = ui->checkBox_SM_msgConf->isChecked()?payload0.saveMask|0x02:payload0.saveMask;
    payload0.saveMask  = ui->checkBox_SM_infMsg->isChecked()?payload0.saveMask|0x04:payload0.saveMask;
    payload0.saveMask  = ui->checkBox_SM_PVTConf->isChecked()?payload0.saveMask|0x08:payload0.saveMask;
    payload0.saveMask  = ui->checkBox_SM_ASConf->isChecked()?payload0.saveMask|0x10:payload0.saveMask;
    payload0.saveMask  = ui->checkBox_SM_ftsConf->isChecked()?payload0.saveMask|0x1000:payload0.saveMask;

    payload0.loadMask  = ui->checkBox_LM_ioPort->isChecked()?payload0.loadMask|0x01:payload0.loadMask;
    payload0.loadMask  = ui->checkBox_LM_msgConf->isChecked()?payload0.loadMask|0x02:payload0.loadMask;
    payload0.loadMask  = ui->checkBox_LM_infMsg->isChecked()?payload0.loadMask|0x04:payload0.loadMask;
    payload0.loadMask  = ui->checkBox_LM_PVTConf->isChecked()?payload0.loadMask|0x08:payload0.loadMask;
    payload0.loadMask  = ui->checkBox_LM_ASConf->isChecked()?payload0.loadMask|0x10:payload0.loadMask;
    payload0.loadMask  = ui->checkBox_LM_ftsConf->isChecked()?payload0.loadMask|0x1000:payload0.loadMask;
    BuildBreamFrame(BRM_STP_CFG,  sizeof(LD2BRM_StpConfigCommandPayload), (unsigned char*) &payload0, ( char*) &buf[0]);
    QByteArray lineText(buf,sizeof(LD2BRM_StpConfigCommandPayload)+8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}

//BRM-STP-CONFIG2 (0x06 0x24)
void ConfigurationLC29D::on_pushButton_GC13_clicked()
{
    LD2BRM_StpConfig2PollPayload payload0;
    memset(&payload0,0,sizeof(payload0));
    char buf[1024] = {0};
    payload0.mask  = ui->checkBox_dyn->isChecked()?payload0.mask|0x01:payload0.mask;
    payload0.mask  = ui->checkBox_posMask->isChecked()?payload0.mask|0x10:payload0.mask;
    payload0.mask  = ui->checkBox_blindGalSearch->isChecked()?payload0.mask|0x800:payload0.mask;
    payload0.context = static_cast<uint8_t>(ui->comboBox_Context->currentText().split(':')[0].toInt());
    payload0.pAcc = ui->lineEdit_pAcc->text().toUShort();
    payload0.blindGalSearch = ui->checkBox_EnableBlindGalSearch->isChecked();
    BuildBreamFrame(BRM_STP_CFG2,  sizeof(LD2BRM_StpConfig2PollPayload), (unsigned char*) &payload0, ( char*) &buf[0]);
    QByteArray lineText(buf,sizeof(LD2BRM_StpConfig2PollPayload)+8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}

void ConfigurationLC29D::on_pushButton_GC14_clicked()
{
    LD2BRM_StpMessagePollPayload payload0;
    memset(&payload0,0,sizeof(payload0));
    char buf[1024] = {0};
    payload0.msgClass  = static_cast<uint8_t>(ui->lineEdit_msgGroup->text().toUShort());
    payload0.msgID     = static_cast<uint8_t>( ui->lineEdit_msgNumber->text().toUShort());
    BuildBreamFrame(BRM_STP_MSG,  sizeof(LD2BRM_StpMessagePollPayload), (unsigned char*) &payload0, ( char*) &buf[0]);
    QByteArray lineText(buf,sizeof(LD2BRM_StpMessagePollPayload)+8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}

void ConfigurationLC29D::on_pushButton_GC15_clicked()
{
    LD2BRM_StpMessageSetsPayload payload0;
    memset(&payload0,0,sizeof(payload0));
    char buf[1024] = {0};
    payload0.msgClass  = static_cast<uint8_t>(ui->lineEdit_msgGroup_rate->text().toUShort());
    payload0.msgID     = static_cast<uint8_t>( ui->lineEdit_msgNumber_rate->text().toUShort());
    payload0.rate[1]   = static_cast<uint8_t>(ui->lineEdit_rate_uart->text().toUShort());
    BuildBreamFrame(BRM_STP_MSG,  sizeof(LD2BRM_StpMessageSetsPayload), (unsigned char*) &payload0, ( char*) &buf[0]);
    QByteArray lineText(buf,sizeof(LD2BRM_StpMessageSetsPayload)+8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}
//Set rate(0x06 0x01)
void ConfigurationLC29D::on_pushButton_GC16_clicked()
{
    LD2BRM_StpMessageSetPayload payload0;
    memset(&payload0,0,sizeof(payload0));
    char buf[1024] = {0};
    //payload0.msgClass  = static_cast<uint8_t>(ui->lineEdit_msgGroup_rate_c->text().toUShort());
    payload0.msgClass  = ui->comboBox_groupAndNumber->currentText().split(' ')[0].toInt(NULL,16);
    payload0.msgID  = ui->comboBox_groupAndNumber->currentText().split(' ')[1].toInt(NULL,16);
    //payload0.msgID     = static_cast<uint8_t>( ui->lineEdit_msgNumber_rate_c->text().toUShort());
    payload0.rate   = static_cast<uint8_t>(ui->lineEdit_rate_c->text().toUShort());
    BuildBreamFrame(BRM_STP_MSG,  sizeof(LD2BRM_StpMessageSetPayload), (unsigned char*) &payload0, ( char*) &buf[0]);
    QByteArray lineText(buf,sizeof(LD2BRM_StpMessageSetPayload)+8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}

//BRM-STP-ME_SETTINGS (0x06 0x3E)
void ConfigurationLC29D::on_pushButton_GC18_clicked()
{
    LD2BRM_StpMeSettingsPollPayload payload0;
    LD2BRM_StpMeSettingsPollPayload_block payload_block;
    QByteArray qbArray;
    memset(&payload0,0,sizeof(payload0));
    char buf[1024] = {0};
    payload0.numTrkChUse = static_cast<uint8_t>(ui->lineEdit_numTrkChUse->text().toUInt());
    qbArray.append((char*)&payload0, sizeof(payload0));
    if(ui->checkBox_GC_GPS->isChecked())
    {
        payload0.numConfigBlocks++;
        memset(&payload_block,0,sizeof(payload_block));
        payload_block.gnssId = 0;
        payload_block.enable = ui->checkBox_Enable_GPS->isChecked()?payload_block.enable|0x1:payload_block.enable;
        payload_block.sigCfgMask = ui->checkBox_GPS_L1->isChecked()?payload_block.sigCfgMask|0x01:payload_block.sigCfgMask;
        payload_block.sigCfgMask = ui->checkBox_GPS_L5->isChecked()?payload_block.sigCfgMask|0x04:payload_block.sigCfgMask;
        qbArray.append((char*)&payload_block, sizeof(payload_block));
    }
    if(ui->checkBox_GC_Gal->isChecked())
    {
        payload0.numConfigBlocks++;
        memset(&payload_block,0,sizeof(payload_block));
        payload_block.gnssId = 2;
        payload_block.enable = ui->checkBox_Enable_Gal->isChecked()?payload_block.enable|0x1:payload_block.enable;
        payload_block.sigCfgMask = ui->checkBox_Gal_E1->isChecked()?payload_block.sigCfgMask|0x01:payload_block.sigCfgMask;
        payload_block.sigCfgMask = ui->checkBox_Gal_E5->isChecked()?payload_block.sigCfgMask|0x04:payload_block.sigCfgMask;
        qbArray.append((char*)&payload_block, sizeof(payload_block));
    }
    if(ui->checkBox_GC_BD->isChecked())
    {
        payload0.numConfigBlocks++;
        memset(&payload_block,0,sizeof(payload_block));
        payload_block.gnssId = 3;
        payload_block.enable = ui->checkBox_Enable_BD->isChecked()?payload_block.enable|0x1:payload_block.enable;
        payload_block.sigCfgMask = ui->checkBox_BD_B1->isChecked()?payload_block.sigCfgMask|0x01:payload_block.sigCfgMask;
        payload_block.sigCfgMask = ui->checkBox_BD_B2aL5->isChecked()?payload_block.sigCfgMask|0x04:payload_block.sigCfgMask;
        qbArray.append((char*)&payload_block, sizeof(payload_block));
    }
    if(ui->checkBox_GC_QZSS->isChecked())
    {
        payload0.numConfigBlocks++;
        memset(&payload_block,0,sizeof(payload_block));
        payload_block.gnssId = 5;
        payload_block.enable = ui->checkBox_Enable_QZSS->isChecked()?payload_block.enable|0x1:payload_block.enable;
        payload_block.sigCfgMask = ui->checkBox_QZSS_L1->isChecked()?payload_block.sigCfgMask|0x01:payload_block.sigCfgMask;
        payload_block.sigCfgMask = ui->checkBox_QZSS_L5->isChecked()?payload_block.sigCfgMask|0x04:payload_block.sigCfgMask;
        qbArray.append((char*)&payload_block, sizeof(payload_block));
    }
    if(ui->checkBox_GC_Glo->isChecked())
    {
        payload0.numConfigBlocks++;
        memset(&payload_block,0,sizeof(payload_block));
        payload_block.gnssId = 6;
        payload_block.enable = ui->checkBox_Enable_Glo->isChecked()?payload_block.enable|0x1:payload_block.enable;
        payload_block.sigCfgMask = ui->checkBox_Glo_L1->isChecked()?payload_block.sigCfgMask|0x01:payload_block.sigCfgMask;
        qbArray.append((char*)&payload_block, sizeof(payload_block));
    }
    if(ui->checkBox_GC_NAVIC->isChecked())
    {
        payload0.numConfigBlocks++;
        memset(&payload_block,0,sizeof(payload_block));
        payload_block.gnssId = 7;
        payload_block.enable = ui->checkBox_Enable_NAVIC->isChecked()?payload_block.enable|0x1:payload_block.enable;
        payload_block.sigCfgMask = ui->checkBox_NAVIC_L5->isChecked()?payload_block.sigCfgMask|0x04:payload_block.sigCfgMask;
        qbArray.append((char*)&payload_block, sizeof(payload_block));
    }
    qbArray[3] = static_cast<char>(payload0.numConfigBlocks);
    BuildBreamFrame(BRM_STP_GNSS,  qbArray.length(), (unsigned char*)qbArray.data(), ( char*) &buf[0]);
    QByteArray lineText(buf,qbArray.length()+8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}
//BRM-STP-NMEA (0x06 0x17)
void ConfigurationLC29D::on_pushButton_GC19_clicked()
{
    LD2BRM_StpNmeaSetDeprecatedPayload payload0 = {0,0,0,0};
    char buf[1024] = {0};
    payload0.nmeaVersion = ui->comboBox_NMEA_Version->currentIndex()?0x41:0x21;
    payload0.numSV       = static_cast<U1>(ui->comboBox_numSV->currentText().split(':')[0].toInt());
    /* 3-2 build frame */
    BuildBreamFrame(BRM_STP_NMEA,  sizeof(LD2BRM_StpNmeaSetDeprecatedPayload), (unsigned char*)&payload0, ( char*) &buf[0]);
    /* 3-3 send frame */
    QByteArray lineText(buf,sizeof(LD2BRM_StpNmeaSetDeprecatedPayload)+8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}

//BRM-STP-PORT (0x06 0x00)
void ConfigurationLC29D::on_pushButton_GC21_clicked()
{
    LD2BRM_StpPortSetUartPayload payload0;
    memset(&payload0,0,sizeof(payload0));
    char buf[1024] = {0};
    payload0.PortID       = static_cast<uint8_t>(ui->comboBox_portID->currentText().split(':')[0].toUInt());
    payload0.outProtoMask = ui->comboBox_protocolOut->currentIndex();
    payload0.baudRate     = ui->comboBox_baudRate->currentText().toUInt();
    payload0.flags        = ui->comboBox_extendedTxTimeout->currentText().toUShort();
    payload0.mode = 0x08C0;
    BuildBreamFrame(BRM_STP_PRT,  sizeof(LD2BRM_StpPortSetUartPayload), (unsigned char*) &payload0, ( char*) &buf[0]);
    QByteArray lineText(buf,sizeof(LD2BRM_StpPortSetUartPayload)+8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}

//BRM-STP-PWR_MODE (0x06 0x86)
void ConfigurationLC29D::on_pushButton_GC22_clicked()
{
    LD2BRM_StpPwrModeSetPayload payload0;
    memset(&payload0,0,sizeof(payload0));
    char buf[1024] = {0};
    payload0.powerSetupValue = static_cast<uint8_t>(ui->comboBox_powerSetupValue->currentIndex());
    payload0.period          = ui->lineEdit_period->text().toUShort();
    payload0.onTime          = ui->lineEdit_onTime->text().toUShort();
    payload0.tryToDecodeEph  = static_cast<uint8_t>(ui->lineEdit_tryToDecodeEph->text().toUInt());
    BuildBreamFrame(BRM_STP_PMS,  sizeof(LD2BRM_StpPwrModeSetPayload), (unsigned char*) &payload0, ( char*) &buf[0]);
    QByteArray lineText(buf,sizeof(LD2BRM_StpPwrModeSetPayload)+8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}

void ConfigurationLC29D::on_pushButton_GC23_clicked()
{
    LD2BRM_StpResetCommandPayload payload0;
    memset(&payload0,0,sizeof(payload0));
    char buf[1024] = {0};
    if(ui->comboBox_reset_option->currentIndex() == 3){
        payload0.navBbrMask = 0xffff;
    }else{
        payload0.navBbrMask = ui->checkBox_Ephemeris->isChecked()?payload0.navBbrMask|0x01:payload0.navBbrMask;
        payload0.navBbrMask = ui->checkBox_Almanac->isChecked()?payload0.navBbrMask|0x02:payload0.navBbrMask;
        payload0.navBbrMask = ui->checkBox_Position->isChecked()?payload0.navBbrMask|0x10:payload0.navBbrMask;
        payload0.navBbrMask = ui->checkBox_Clockdrift->isChecked()?payload0.navBbrMask|0x20:payload0.navBbrMask;
        payload0.navBbrMask = ui->checkBox_UTC->isChecked()?payload0.navBbrMask|0x80:payload0.navBbrMask;
        payload0.navBbrMask = ui->checkBox_RTC->isChecked()?payload0.navBbrMask|0x100:payload0.navBbrMask;
        payload0.navBbrMask = ui->checkBox_cbee->isChecked()?payload0.navBbrMask|0x8000:payload0.navBbrMask;
    }
    payload0.resetMode  = static_cast<uint8_t>(ui->comboBox_resetMode->currentText().split(':')[0].toUInt());
    BuildBreamFrame(BRM_STP_RST,  sizeof(LD2BRM_StpResetCommandPayload), (unsigned char*) &payload0, ( char*) &buf[0]);
    QByteArray lineText(buf,sizeof(LD2BRM_StpResetCommandPayload)+8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}

//BRM-STP-RF_L1L5bias (0x06 0xB0)
void ConfigurationLC29D::on_pushButton_GC24_clicked()
{
    LD2BRM_StpBiasPollPayload payload0;
    memset(&payload0,0,sizeof(payload0));
    char buf[1024] = {0};
    payload0.L1L5bias = static_cast<uint32_t>(ui->lineEdit_L1L5bias->text().toDouble() * 100);
    BuildBreamFrame(BRM_STP_BIAS,  sizeof(LD2BRM_StpBiasPollPayload), (unsigned char*) &payload0, ( char*) &buf[0]);
    QByteArray lineText(buf,sizeof(LD2BRM_StpBiasPollPayload)+8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}

//BRM-STP-PPS(0x06 0x31)
void ConfigurationLC29D::on_pushButton_GC25_clicked()
{
    LD2BRM_StpPPSPollPayload payload0;
    memset(&payload0,0,sizeof(payload0));
    char buf[1024] = {0};
    payload0.version = 0x01;
    payload0.flags = ui->checkBox_EnableTimePulse->isChecked()?payload0.flags|0x01:payload0.flags;
    payload0.flags = ui->radioButton_isFreq->isChecked()?payload0.flags|0x08:payload0.flags;
    payload0.flags = ui->radioButton_isLength->isChecked()?payload0.flags|0x10:payload0.flags;
    payload0.flags = ui->checkBox_RisingDege->isChecked()?payload0.flags|0x40:payload0.flags;
    payload0.freqPeriod = ui->lineEdit_freqPeriod->text().toUInt();
    payload0.pulseLenRatio = ui->lineEdit_pulseLenRatio->text().toUInt();
    BuildBreamFrame(BRM_STP_PPS,  sizeof(LD2BRM_StpPPSPollPayload), (unsigned char*) &payload0, ( char*) &buf[0]);
    QByteArray lineText(buf,sizeof(LD2BRM_StpPPSPollPayload)+8);
    ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
}

void ConfigurationLC29D::on_radioButton_isFreq_toggled(bool checked)
{
    if(checked)
    {
        ui->label_freqPeriod->setText("Pulse frequency(Hz)");
    }
    else
    {
        ui->label_freqPeriod->setText("Pulse period(us)");
    }
}

void ConfigurationLC29D::on_radioButton_isLength_toggled(bool checked)
{
    if(checked)
    {
        ui->label_pulseLenRatio->setText("Pulse length(us)");
    }
    else
    {
        ui->label_pulseLenRatio->setText("Pulse duty cycle(%)");
    }
}

void ConfigurationLC29D::on_comboBox_reset_option_currentIndexChanged(int index)
{
    if(index == 0){
        ui->checkBox_Ephemeris->setEnabled(true);
        ui->checkBox_Almanac->setEnabled(true);
        ui->checkBox_Position->setEnabled(true);
        ui->checkBox_Clockdrift->setEnabled(true);
        ui->checkBox_UTC->setEnabled(true);
        ui->checkBox_RTC->setEnabled(true);
        ui->checkBox_cbee->setEnabled(true);
    }else if(index == 1){
        ui->checkBox_Ephemeris->setChecked(false);
        ui->checkBox_Almanac->setChecked(false);
        ui->checkBox_Position->setChecked(false);
        ui->checkBox_Clockdrift->setChecked(false);
        ui->checkBox_UTC->setChecked(false);
        ui->checkBox_RTC->setChecked(false);
        ui->checkBox_cbee->setChecked(false);
        //disabled
        ui->checkBox_Ephemeris->setEnabled(false);
        ui->checkBox_Almanac->setEnabled(false);
        ui->checkBox_Position->setEnabled(false);
        ui->checkBox_Clockdrift->setEnabled(false);
        ui->checkBox_UTC->setEnabled(false);
        ui->checkBox_RTC->setEnabled(false);
        ui->checkBox_cbee->setEnabled(false);
    }else if(index == 2){
        ui->checkBox_Ephemeris->setChecked(true);
        ui->checkBox_Almanac->setChecked(false);
        ui->checkBox_Position->setChecked(false);
        ui->checkBox_Clockdrift->setChecked(false);
        ui->checkBox_UTC->setChecked(false);
        ui->checkBox_RTC->setChecked(false);
        ui->checkBox_cbee->setChecked(false);
        //disabled
        ui->checkBox_Ephemeris->setEnabled(false);
        ui->checkBox_Almanac->setEnabled(false);
        ui->checkBox_Position->setEnabled(false);
        ui->checkBox_Clockdrift->setEnabled(false);
        ui->checkBox_UTC->setEnabled(false);
        ui->checkBox_RTC->setEnabled(false);
        ui->checkBox_cbee->setEnabled(false);
    }else if(index == 3){
        ui->checkBox_Ephemeris->setChecked(true);
        ui->checkBox_Almanac->setChecked(true);
        ui->checkBox_Position->setChecked(true);
        ui->checkBox_Clockdrift->setChecked(true);
        ui->checkBox_UTC->setChecked(true);
        ui->checkBox_RTC->setChecked(true);
        ui->checkBox_cbee->setChecked(true);
        //disabled
        ui->checkBox_Ephemeris->setEnabled(false);
        ui->checkBox_Almanac->setEnabled(false);
        ui->checkBox_Position->setEnabled(false);
        ui->checkBox_Clockdrift->setEnabled(false);
        ui->checkBox_UTC->setEnabled(false);
        ui->checkBox_RTC->setEnabled(false);
        ui->checkBox_cbee->setEnabled(false);
    }
}

//Get current settings
void ConfigurationLC29D::on_pushButton_poll_clicked()
{
    char buf[1024] = {0};
    QByteArray lineText;
    switch (ui->stackedWidget->currentIndex())
    {
    //BRM-STP-CONFIG2 (0x06 0x24)
    case 13:
    {
        LD2BRM_StpConfig2PollPayload payload0;
        memset(&payload0,0,sizeof(payload0));
        BuildBreamFrame(BRM_STP_CFG2,  0, (unsigned char*) &payload0, ( char*) &buf[0]);
        lineText.append(buf,8);
        emit sendData(lineText.data(), lineText.size());
        ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
        break;
    }
    // Poll a Message Setup
    case 16:
    {
        LD2BRM_StpMessageSetPayload payload0;
        memset(&payload0,0,sizeof(payload0));
        payload0.msgClass  = ui->comboBox_groupAndNumber->currentText().split(' ')[0].toInt(NULL,16);
        payload0.msgID  = ui->comboBox_groupAndNumber->currentText().split(' ')[1].toInt(NULL,16);
        BuildBreamFrame(BRM_STP_MSG,  2, (unsigned char*) &payload0, ( char*) &buf[0]);
        lineText.append(buf,8+2);
        emit sendData(lineText.data(), lineText.size());
        ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
        break;
    }
    //BRM-STP-ME_SETTINGS (0x06 0x3E)
    case 18:
    {
        LD2BRM_StpMeSettingsPollPayload payload0;
        memset(&payload0,0,sizeof(payload0));
        BuildBreamFrame(BRM_STP_GNSS,  0, (unsigned char*) &payload0, ( char*) &buf[0]);
        lineText.append(buf,8);
        emit sendData(lineText.data(), lineText.size());
        ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
        break;
    }
    //BRM-STP-NMEA (0x06 0x17)
//    case 19:
//    {
//        LD2BRM_StpNmeaSetDeprecatedPayload payload0 = {0,0,0,0};
//        BuildBreamFrame(BRM_STP_NMEA,  0, (unsigned char*)&payload0, ( char*) &buf[0]);
//        lineText.append(buf,8);
//        emit sendData(lineText.data(), lineText.size());
//        ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
//        break;
//    }
    //BRM-STP-PORT (0x06 0x00)
    case 21:
    {
        LD2BRM_StpPortSetUartPayload payload0;
        memset(&payload0,0,sizeof(payload0));
        payload0.PortID = 1;
        BuildBreamFrame(BRM_STP_PRT,  1, (unsigned char*) &payload0, ( char*) &buf[0]);
        lineText.append(buf,9);
        emit sendData(lineText.data(), lineText.size());
        ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
        break;
    }
    //BRM-STP-PORT (0x06 0x86)
    case 22:
    {
        LD2BRM_StpPwrModeSetPayload payload0;
        memset(&payload0,0,sizeof(payload0));
        BuildBreamFrame(BRM_STP_PMS,  0, (unsigned char*) &payload0, ( char*) &buf[0]);
        lineText.append(buf,8);
        emit sendData(lineText.data(), lineText.size());
        ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
        break;
    }
    //BRM-STP-RF_L1L5bias (0x06 0xB0)
    case 24:
    {
        LD2BRM_StpBiasPollPayload payload0;
        memset(&payload0,0,sizeof(payload0));
        BuildBreamFrame(BRM_STP_BIAS,  0, (unsigned char*) &payload0, ( char*) &buf[0]);
        lineText.append(buf,8);
        emit sendData(lineText.data(), lineText.size());
        ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
        break;
    }
    //BRM-STP-PPS(0x06 0x31)
    case 25:
    {
        LD2BRM_StpPPSPollPayload payload0;
        memset(&payload0,0,sizeof(payload0));
        BuildBreamFrame(BRM_STP_PPS,  0, (unsigned char*) &payload0, ( char*) &buf[0]);
        lineText.append(buf,8);
        emit sendData(lineText.data(), lineText.size());
        ui->lineEditCommand->setText(lineText.toHex(' ').toUpper());
        break;
    }
    default:
    {
        break;
    }
    }
}

void ConfigurationLC29D::parseBreamMsg(QQueue<QByteArray> &breamSets_Q)
{
    while(!breamSets_Q.isEmpty())
    {
        QByteArray mge = breamSets_Q.takeFirst();
        QDateTime current_time    = QDateTime::currentDateTime();
        if(mge.at(2) == 0x05)
        {
            QString messages;
            if(mge.at(3))
            {
                messages = QString("Message(0x%1 0x%2) sent successfully")
                        .arg((uint8_t)mge.at(6),2,16,QChar('0'))
                        .arg((uint8_t)mge.at(7),2,16,QChar('0'));
            }
            else{
                messages = "Configuration failed";
            }
            messages = current_time.toString("(hh:mm:ss) ") + messages;
            statusBar->showMessage(messages);
        }
        //BRM-STP-CONFIG2 (0x06 0x24)
        else if(mge.at(2) == 0x06 && mge.at(3) == 0x24)
        {
            ui->lineEdit_pAcc->setText(QString::number((static_cast<uint8_t>(mge.at(18+6+1)) << 8) +
                                                       static_cast<uint8_t>(mge.at(18+6))));
            ui->checkBox_dyn->setChecked(mge.at(6)&0x01);
            ui->checkBox_posMask->setChecked(mge.at(6)&0x10);
            ui->checkBox_blindGalSearch->setChecked(mge.at(7)&0x08);
            ui->checkBox_EnableBlindGalSearch->setChecked(mge.at(31+6));
            for (int i = 0;i < ui->comboBox_Context->count() ;i++) {
                if(ui->comboBox_Context->itemText(i).split(':')[0].toInt() == mge.at(2+6))
                {
                    ui->comboBox_Context->setCurrentIndex(i);
                    break;
                }
            }
        }
        //Poll a Message Setup
        else if(mge.at(2) == 0x06 && mge.at(3) == 0x01)
        {
            ui->lineEdit_rate_c->setText(QString::number(mge.at(9)));
        }
        //BRM-STP-ME_SETTINGS (0x06 0x3E)
        else if(mge.at(2) == 0x06 && mge.at(3) == 0x3E)
        {
            ui->lineEdit_numTrkChUse->setText(QString::number(mge.at(2+6)));
            ui->lineEdit_numTrkChHw->setText(QString::number(mge.at(1+6)));
            for (int i = 0; i < mge.at(3 + 6) ; i++) {
                if(mge.at(6 + 4 + 8 * i) == 0){
                    ui->checkBox_Enable_GPS->setChecked(mge.at(6 + 8 + 8 * i)&0x01);
                    ui->checkBox_GPS_L1->setChecked(mge.at(6 + 8 + 8 * i +2)&0x01);
                    ui->checkBox_GPS_L5->setChecked(mge.at(6 + 8 + 8 * i +2)&0x04);
                }
                else if(mge.at(6 + 4 + 8 * i) == 2){
                    ui->checkBox_Enable_Gal->setChecked(mge.at(6 + 8 + 8 * i)&0x01);
                    ui->checkBox_Gal_E1->setChecked(mge.at(6 + 8 + 8 * i +2)&0x01);
                    ui->checkBox_Gal_E5->setChecked(mge.at(6 + 8 + 8 * i +2)&0x04);
                }
                else if(mge.at(6 + 4 + 8 * i) == 3){
                    ui->checkBox_Enable_BD->setChecked(mge.at(6 + 8 + 8 * i)&0x01);
                    ui->checkBox_BD_B1->setChecked(mge.at(6 + 8 + 8 * i +2)&0x01);
                    ui->checkBox_BD_B2aL5->setChecked(mge.at(6 + 8 + 8 * i +2)&0x04);
                }
                else if(mge.at(6 + 4 + 8 * i) == 5){
                    ui->checkBox_Enable_QZSS->setChecked(mge.at(6 + 8 + 8 * i)&0x01);
                    ui->checkBox_QZSS_L1->setChecked(mge.at(6 + 8 + 8 * i +2)&0x01);
                    ui->checkBox_QZSS_L5->setChecked(mge.at(6 + 8 + 8 * i +2)&0x04);
                }
                else if(mge.at(6 + 4 + 8 * i) == 6){
                    ui->checkBox_Enable_Glo->setChecked(mge.at(6 + 8 + 8 * i)&0x01);
                    ui->checkBox_Glo_L1->setChecked(mge.at(6 + 8 + 8 * i +2)&0x01);
                }
                else if(mge.at(6 + 4 + 8 * i) == 7){
                    ui->checkBox_Enable_NAVIC->setChecked(mge.at(6 + 8 + 8 * i)&0x01);
                    ui->checkBox_NAVIC_L5->setChecked(mge.at(6 + 8 + 8 * i +2)&0x04);
                }
            }
        }
        //BRM-STP-NMEA (0x06 0x17)
//        else if(mge.at(2) == 0x06 && mge.at(3) == 0x17)
//        {
//            ui->comboBox_NMEA_Version->setCurrentIndex(mge.at(1+6) == 0x41);
//            for (int i = 0;i < ui->comboBox_numSV->count() ;i++) {
//                if(ui->comboBox_numSV->itemText(i).split(':')[0].toInt() == mge.at(2+6))
//                {
//                    ui->comboBox_numSV->setCurrentIndex(i);
//                    break;
//                }
//            }
//        }
        //BRM-STP-NMEA (0x06 0x00)
        else if(mge.at(2) == 0x06 && mge.at(3) == 0x00)
        {
            uint32_t baudRate_tmp = static_cast<uint32_t>((static_cast<uint8_t>(mge.at(6 + 11))<<24) +
                    (static_cast<uint8_t>(mge.at(6 + 10))<<16) +
                    (static_cast<uint8_t>(mge.at(6 + 9))<<8) +
                    static_cast<uint8_t>(mge.at(6 + 8)));
            if(baudRate_tmp == 115200){
                ui->comboBox_baudRate->setCurrentIndex(0);
            }
            else if(baudRate_tmp == 230400){
                ui->comboBox_baudRate->setCurrentIndex(1);
            }
            else if(baudRate_tmp == 460800){
                ui->comboBox_baudRate->setCurrentIndex(2);
            }
            ui->comboBox_extendedTxTimeout->setCurrentIndex(mge.at(6 + 16)&0x02);
            ui->comboBox_protocolOut->setCurrentIndex(mge.at(6 + 14));
        }
        //BRM-STP-NMEA (0x06 0x86)
        else if(mge.at(2) == 0x06 && static_cast<uint8_t>(mge.at(3)) == 0x86)
        {
            ui->comboBox_powerSetupValue->setCurrentIndex(mge.at(6 + 1));
            ui->lineEdit_period->setText(QString::number(
                                             (static_cast<uint8_t>(mge.at(6 + 3))<<8)+
                                              static_cast<uint8_t>(mge.at(6 + 2))
                                             ));
            ui->lineEdit_onTime->setText(QString::number(
                                             (static_cast<uint8_t>(mge.at(6 + 5))<<8)+
                                              static_cast<uint8_t>(mge.at(6 + 4))
                                             ));
            ui->lineEdit_tryToDecodeEph->setText(QString::number(
                                                     (static_cast<uint8_t>(mge.at(6 + 7))<<8)+
                                                      static_cast<uint8_t>(mge.at(6 + 6))
                                                     ));
        }
        //BRM-STP-NMEA (0x06 0xB0)
        else if(mge.at(2) == 0x06 && static_cast<uint8_t>(mge.at(3)) == 0xB0)
        {
            ui->lineEdit_L1L5bias->setText(QString::number(
                                               ((static_cast<uint8_t>(mge.at(6 + 3))<<24)+
                                               (static_cast<uint8_t>(mge.at(6 + 2))<<16)+
                                               (static_cast<uint8_t>(mge.at(6 + 1))<<8)+
                                                static_cast<uint8_t>(mge.at(6)))/100.0
                                               ));
        }
        //BRM-STP-PPS(0x06 0x31)
        else if(mge.at(2) == 0x06 && static_cast<uint8_t>(mge.at(3)) == 0x31)
        {
            if(mge.size()<35) return;
            ui->checkBox_EnableTimePulse->setChecked(0x01&static_cast<uint8_t>(mge.at(34)));
            ui->radioButton_isFreq->setChecked(0x08&static_cast<uint8_t>(mge.at(34)));
            ui->radioButton_isLength->setChecked(0x10&static_cast<uint8_t>(mge.at(34)));
            ui->checkBox_RisingDege->setChecked(0x40&static_cast<uint8_t>(mge.at(34)));
            ui->lineEdit_freqPeriod->setText(QString::number(
                                                 (static_cast<uint8_t>(mge.at(14 + 3))<<24)+
                                                 (static_cast<uint8_t>(mge.at(14 + 2))<<16)+
                                                 (static_cast<uint8_t>(mge.at(14 + 1))<<8)+
                                                  static_cast<uint8_t>(mge.at(14))
                                                 ));
            ui->lineEdit_pulseLenRatio->setText(QString::number(
                                                    (static_cast<uint8_t>(mge.at(22 + 3))<<24)+
                                                    (static_cast<uint8_t>(mge.at(22 + 2))<<16)+
                                                    (static_cast<uint8_t>(mge.at(22 + 1))<<8)+
                                                     static_cast<uint8_t>(mge.at(22))
                                                    ));

        }

    }
}

//$PQVERNO
//$PQVERNO
void ConfigurationLC29D::on_pushButton_GC2_clicked()
{
    QByteArray commant_str("$PQTMVERNO\r\n");
    qDebug()<<commant_str.data();
    emit sendData(commant_str.data(), commant_str.size());
}

void ConfigurationLC29D::on_pushButton_CurrentTime_clicked()
{
    QDateTime cTime = QDateTime::currentDateTime().toUTC();
    ui->lineEdit_year->setText(cTime.toString("yyyy"));
    ui->lineEdit_month->setText(cTime.toString("MM"));
    ui->lineEdit_day->setText(cTime.toString("dd"));
    ui->lineEdit_hour->setText(cTime.toString("hh"));
    ui->lineEdit_minute->setText(cTime.toString("mm"));
    ui->lineEdit_second->setText(cTime.toString("ss"));
}

void ConfigurationLC29D::keyPressEvent(QKeyEvent *ev)
{
    if(ev->modifiers() == (Qt::ShiftModifier|Qt::AltModifier))
    {
        if(ev->key() == Qt::Key_Q)
        {
            QTreeWidgetItemIterator it(ui->treeWidget);
            while (*it) {
                (*it)->setHidden(false);
                ++it;
            }
            statusBar->showMessage("Superuser mode.");
        }
    }
}

void ConfigurationLC29D::on_pushButton_PQTMCFGEINSMSG_clicked()
{
    QByteArray cmd("$PQTMCFGEINSMSG,1,");
    cmd.append(QString::number(ui->comboBox_ins_onOff->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_imu_onOff->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_gps_onOff->currentIndex()));
    cmd.append(",");
    cmd.append(ui->comboBox_imu_ins_rate->currentText().split(' ')[0]);
    addNMEA_MC_CS(cmd);
    cmd.append("\r\n");
    emit sendData(cmd.data(), cmd.size());
}

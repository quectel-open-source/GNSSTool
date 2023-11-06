#include "configuration_L26DR.h"
#include "ui_configuration_L26DR.h"
#include <QDebug>
///Do not support this command
/// L26-DR (UDR/ADR/ADRC):PSTMGEOFENCEREQ,PSTMCFGGEOFENCE,
/// L26-DR (UDR/ADRC):PSTMODOSTART,PSTMODOSTOP,PSTMODOREQ,PSTMODORESET,PSTMCFGODO
/// L26-P:PSTMGEOFENCEREQ,
///
/// Only L26-DR (UDR/ADR/ADRC) and L26-P support this command:PSTMIMUSELFTESTCMD,
/// Only L26-P module supports this command: PSTMCFGGEOCIR,

configuration_L26DR::configuration_L26DR(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configuration_L26DR)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->treeWidget->setColumnWidth(0,400);
    this->setAttribute(Qt::WA_QuitOnClose,false);
    ui->treeWidget->hideColumn(1);
}

configuration_L26DR::~configuration_L26DR()
{
    delete ui;
}
void configuration_L26DR::setReceiver_type(const quectel_gnss::QGNSS_Model &value)
{
    receiver_type = value;
    if(receiver_type == quectel_gnss::L26ADR)
    {
        titleName="Configuration:L26ADR";
        QList<QTreeWidgetItem*> s = ui->treeWidget->findItems("PSTMGEOFENCEREQ", Qt::MatchContains | Qt::MatchRecursive);
        s.append(ui->treeWidget->findItems("PSTMCFGGEOFENCE", Qt::MatchContains | Qt::MatchRecursive));
        s.append(ui->treeWidget->findItems("PSTMCFGGEOCIR", Qt::MatchContains | Qt::MatchRecursive));
        for (auto item : s) {
            item->setHidden(true);
        }
    }else if(receiver_type == quectel_gnss::L26UDR){
        titleName="Configuration:L26UDR";
        QList<QTreeWidgetItem*> s = ui->treeWidget->findItems("PSTMGEOFENCEREQ", Qt::MatchContains | Qt::MatchRecursive);
        s.append(ui->treeWidget->findItems("PSTMCFGGEOFENCE", Qt::MatchContains | Qt::MatchRecursive));
        s.append(ui->treeWidget->findItems("PSTMCFGGEOCIR", Qt::MatchContains | Qt::MatchRecursive));
        s.append(ui->treeWidget->findItems("PSTMODOSTART", Qt::MatchContains | Qt::MatchRecursive));
        s.append(ui->treeWidget->findItems("PSTMODOSTOP", Qt::MatchContains | Qt::MatchRecursive));
        s.append(ui->treeWidget->findItems("PSTMODOREQ", Qt::MatchContains | Qt::MatchRecursive));
        s.append(ui->treeWidget->findItems("PSTMODORESET", Qt::MatchContains | Qt::MatchRecursive));
        s.append(ui->treeWidget->findItems("PSTMCFGODO", Qt::MatchContains | Qt::MatchRecursive));
        for (auto item : s) {
            item->setHidden(true);
        }
    }else if(receiver_type == quectel_gnss::L26ADRC){
        titleName="Configuration:L26ADRC";
        QList<QTreeWidgetItem*> s = ui->treeWidget->findItems("PSTMGEOFENCEREQ", Qt::MatchContains | Qt::MatchRecursive);
        s.append(ui->treeWidget->findItems("PSTMCFGGEOFENCE", Qt::MatchContains | Qt::MatchRecursive));
        s.append(ui->treeWidget->findItems("PSTMCFGGEOCIR", Qt::MatchContains | Qt::MatchRecursive));
        s.append(ui->treeWidget->findItems("PSTMODOSTART", Qt::MatchContains | Qt::MatchRecursive));
        s.append(ui->treeWidget->findItems("PSTMODOSTOP", Qt::MatchContains | Qt::MatchRecursive));
        s.append(ui->treeWidget->findItems("PSTMODOREQ", Qt::MatchContains | Qt::MatchRecursive));
        s.append(ui->treeWidget->findItems("PSTMODORESET", Qt::MatchContains | Qt::MatchRecursive));
        s.append(ui->treeWidget->findItems("PSTMCFGODO", Qt::MatchContains | Qt::MatchRecursive));
        for (auto item : s) {
            item->setHidden(true);
        }
    }else if(receiver_type == quectel_gnss::L26P){
        titleName="Configuration:L26P";
        QList<QTreeWidgetItem*> s = ui->treeWidget->findItems("PSTMGEOFENCEREQ", Qt::MatchContains | Qt::MatchRecursive);
        s.append(ui->treeWidget->findItems("PSTMCFGGEOFENCE", Qt::MatchContains | Qt::MatchRecursive));
        for (auto item : s) {
            item->setHidden(true);
        }
    }else if(receiver_type == quectel_gnss::L26T){
        titleName="Configuration:L26T";
        QList<QTreeWidgetItem*> s = ui->treeWidget->findItems("PSTMIMUSELFTESTCMD", Qt::MatchContains | Qt::MatchRecursive);
        s.append(ui->treeWidget->findItems("PSTMCFGGEOCIR", Qt::MatchContains | Qt::MatchRecursive));
        for (auto item : s) {
            item->setHidden(true);
        }
    }else if(receiver_type == quectel_gnss::LC98S){
        titleName="Configuration:LC98S";
    }else if(receiver_type == quectel_gnss::L26DRAA){
        titleName="Configuration:L26DRAA";
    }
    this->setWindowTitle(titleName);
}
void configuration_L26DR::updateUI(QByteArray msg)
{
    QString tmp_L26DR = ui->stackedWidget->currentWidget()->objectName();
    QString tmp = QString(msg);
    QList<QString> sFields = tmp.split('*')[0].split(',');

    if(sFields.count()>3)
    {
        if(tmp_L26DR == "page_PSTMGETRTCTIME")
        {
           ui->lineEdit_PSTMGETRTCTIME_Time->setText(sFields[1]);
           ui->lineEdit_PSTMGETRTCTIME_Date->setText(sFields[2]);
           switch (sFields[3].toInt()) {
           case 0:
               ui->lineEdit_PSTMGETRTCTIME_RtcStatus->setText("0 = RTC_STATUS_INVALID");
               break;
           case 1:
               ui->lineEdit_PSTMGETRTCTIME_RtcStatus->setText("1 = RTC_STATUS_STORED");
               break;
           case 2:
               ui->lineEdit_PSTMGETRTCTIME_RtcStatus->setText("2 = RTC_STATUS_APPROXIMATE");
               break;
           default:
               break;
           }
           //
           switch (sFields[4].toInt()){
           case 0:
               ui->lineEdit_PSTMGETRTCTIME_TimeValidity->setText("0 = NO_TIME");
               break;
           case 1:
               ui->lineEdit_PSTMGETRTCTIME_TimeValidity->setText("1 = FLASH_TIME");
               break;
           case 2:
               ui->lineEdit_PSTMGETRTCTIME_TimeValidity->setText("2 = TOW_TIME");
               break;
           case 3:
               ui->lineEdit_PSTMGETRTCTIME_TimeValidity->setText("3 = USER_TIME");
               break;
           case 4:
               ui->lineEdit_PSTMGETRTCTIME_TimeValidity->setText("4 = USER_RTC_TIME");
               break;
           case 5:
               ui->lineEdit_PSTMGETRTCTIME_TimeValidity->setText("5 = RTC_TIME");
               break;
           case 6:
               ui->lineEdit_PSTMGETRTCTIME_TimeValidity->setText("6 = RTC_TIME_ACCURATE");
               break;
           case 7:
               ui->lineEdit_PSTMGETRTCTIME_TimeValidity->setText("7 = APPROX_TIME");
               break;
           case 8:
               ui->lineEdit_PSTMGETRTCTIME_TimeValidity->setText("8 = ACCURATE_TIME");
               break;
           case 9:
               ui->lineEdit_PSTMGETRTCTIME_TimeValidity->setText("9 = POSITION_TIME");
               break;
           case 10:
               ui->lineEdit_PSTMGETRTCTIME_TimeValidity->setText("10 = EPHEMERIS_TIME");
               break;
           default:
               break;
           }
        }else if(tmp_L26DR =="page_PPS_IF_PULSE_DATA_CMD"){

            switch (sFields[3].toInt()) {
            case 0:
                ui->lineEdit_PULSE_OutMode->setText("0 = PPS always generated");
                break;
            case 1:
                ui->lineEdit_PULSE_OutMode->setText("1 = PPS generated on even seconds");
                break;
            case 2:
                ui->lineEdit_PULSE_OutMode->setText("2 = PPS generated on odd seconds");
                break;
            default:
                break;
            }
            switch (sFields[4].toInt()){
            case 0:
                ui->lineEdit_PULSE_ReferenceTime->setText("0 = UTC");
                break;
            case 1:
                ui->lineEdit_PULSE_ReferenceTime->setText("1 = GPS_UTC");
                break;
            case 2:
                ui->lineEdit_PULSE_ReferenceTime->setText("2 = GLONASS_UTC");
                break;
            case 3:
                ui->lineEdit_PULSE_ReferenceTime->setText("3 = UTC_SU");
                break;
            case 4:
                ui->lineEdit_PULSE_ReferenceTime->setText("4 = GPS_UTC_FROM_GLONASS");
                break;
            case 5:
                ui->lineEdit_PULSE_ReferenceTime->setText("5 = BDS_UTC");
                break;
            case 6:
                ui->lineEdit_PULSE_ReferenceTime->setText("6 = UTC_NTSC");
                break;
            case 7:
                ui->lineEdit_PULSE_ReferenceTime->setText("7 = Galileo");
                break;
            case 8:
                ui->lineEdit_PULSE_ReferenceTime->setText("8 = UTC_Galileo");
                break;
            case 9:
                ui->lineEdit_PULSE_ReferenceTime->setText("9 = GPS_FROM_Galileo");
                break;
            default:
                break;
            }
            ui->lineEdit_PULSE_PulseDelay->setText(sFields[5]);
            ui->lineEdit_PULSE_PulseDuration->setText(sFields[6]);
            switch (sFields[7].toInt()) {
            case 0:
                ui->lineEdit_PULSE_PulsePolarity->setText("0 = Not inverted");
                break;
            case 1:
                ui->lineEdit_PULSE_PulsePolarity->setText("1 = Inverted");
                break;
            default:
                break;
            }

        }else if(tmp_L26DR =="page_PPS_IF_TIMING_DATA_CMD"){

            switch (sFields[3].toInt()) {
            case 0:
                ui->lineEdit_TIMING_FixCondition->setText("0 = No fix");
                break;
            case 1:
                ui->lineEdit_TIMING_FixCondition->setText("1 = No fix");
                break;
            case 2:
                ui->lineEdit_TIMING_FixCondition->setText("2 = 2D fix");
                break;
            case 3:
                ui->lineEdit_TIMING_FixCondition->setText("3 = 3D fix");
                break;
            default:
                break;
            }
            ui->lineEdit_TIMING_SatTh->setText(sFields[4]);
            ui->lineEdit_TIMING_ElevationMask->setText(sFields[5]);
            ui->lineEdit_TIMING_ConstellationMask->setText(sFields[6]);
            ui->lineEdit_TIMING_GPS_RF_Delay->setText(sFields[7]);
            ui->lineEdit_TIMING_GLONSS_RF_Delay->setText(sFields[8]);

        }else if(tmp_L26DR =="page_PPS_IF_POSITION_HOLD_DATA_CMD"){

            switch (sFields[3].toInt()) {
            case 0:
                ui->lineEdit_PPS_HOLD_OnOff->setText("0 = Position hold disabled");
                break;
            case 1:
                ui->lineEdit_PPS_HOLD_OnOff->setText("1 = Position hold enabled");
                break;
            default:
                break;
            }
            ui->lineEdit_PPS_HOLD_Lat->setText(sFields[4]);
            switch (sFields[5].toInt()) {
            case 0:
                ui->lineEdit_PPS_HOLD_NS->setText("N = North");
                break;
            case 1:
                ui->lineEdit_PPS_HOLD_NS->setText("S = South");
                break;
            default:
                break;
            }
            ui->lineEdit_PPS_HOLD_Lon->setText(sFields[6]);
            switch (sFields[7].toInt()) {
            case 0:
                ui->lineEdit_PPS_HOLD_EW->setText("N = North");
                break;
            case 1:
                ui->lineEdit_PPS_HOLD_EW->setText("S = South");
                break;
            default:
                break;
            }
            ui->lineEdit_PPS_HOLD_H_Msl->setText(sFields[8]);
        }else if(tmp_L26DR =="page_PPS_IF_TRAIM_CMD"){

            switch (sFields[3].toInt()) {
            case 0:
                ui->lineEdit_TRAIM_TraimEnabled->setText("0 = OFF");
                break;
            case 1:
                ui->lineEdit_TRAIM_TraimEnabled->setText("1 = ON");
                break;
            default:
                break;
            }
            switch (sFields[4].toInt()) {
            case 0:
                ui->lineEdit_TRAIM_TraimSolution->setText("0 = UNDER alarm");
                break;
            case 1:
                ui->lineEdit_TRAIM_TraimSolution->setText("1 = OVER alarm");
                break;
            case 2:
                ui->lineEdit_TRAIM_TraimSolution->setText("2 = UNKNOWN");
                break;
            default:
                break;
            }
            ui->lineEdit_TRAIM_AveError->setText(sFields[5]);
            ui->lineEdit_TRAIM_UsedSats->setText(sFields[6]);
            ui->lineEdit_TRAIM_RemovedSats->setText(sFields[7]);
        }else if(tmp_L26DR =="page_PPS_IF_POSITION_HOLD_DATA_CMD"){

            switch (sFields[3].toInt()) {
            case 0:
                ui->lineEdit_PPS_HOLD_OnOff->setText("0 = Position hold disabled");
                break;
            case 1:
                ui->lineEdit_PPS_HOLD_OnOff->setText("1 = Position hold enabled");
                break;
            default:
                break;
            }
            ui->lineEdit_PPS_HOLD_Lat->setText(sFields[4]);
            switch (sFields[5].toInt()) {
            case 0:
                ui->lineEdit_PPS_HOLD_NS->setText("N = North");
                break;
            case 1:
                ui->lineEdit_PPS_HOLD_NS->setText("S = South");
                break;
            default:
                break;
            }
            ui->lineEdit_PPS_HOLD_Lon->setText(sFields[6]);
            switch (sFields[7].toInt()) {
            case 0:
                ui->lineEdit_PPS_HOLD_EW->setText("N = North");
                break;
            case 1:
                ui->lineEdit_PPS_HOLD_EW->setText("S = South");
                break;
            default:
                break;
            }
            ui->lineEdit_PPS_HOLD_H_Msl->setText(sFields[8]);
        }else if(tmp_L26DR =="page_PPS_IF_TRAIM_USED_CMD"){

            switch (sFields[3].toInt()) {
            case 0:
                ui->lineEdit_TRAIM_USED_TraimEnabled->setText("0 = OFF");
                break;
            case 1:
                ui->lineEdit_TRAIM_USED_TraimEnabled->setText("1 = ON");
                break;
            default:
                break;
            }
            ui->lineEdit_TRAIM_USED_UsedSats->setText(sFields[4]);
            QString tmp=sFields[5];
            for (int i=6; i<sFields.count(); i++) {
                tmp.append(",");
                tmp.append(sFields[i]);
            }
            ui->lineEdit_TRAIM_USED_SatN->setText(tmp);
        }else if(tmp_L26DR =="page_PPS_IF_TRAIM_RES_CMD"){

            switch (sFields[3].toInt()) {
            case 0:
                ui->lineEdit_TRAIM_RES_TraimEnabled->setText("0 = OFF");
                break;
            case 1:
                ui->lineEdit_TRAIM_RES_TraimEnabled->setText("1 = ON");
                break;
            default:
                break;
            }
            ui->lineEdit_TRAIM_RES_UsedSats->setText(sFields[4]);
            QString tmp=sFields[5];
            for (int i=6; i<sFields.count(); i++) {
                tmp.append(",");
                tmp.append(sFields[i]);
            }
            ui->lineEdit_TRAIM_RES_ResN->setText(tmp);
        }else if(tmp_L26DR =="page_PPS_IF_TRAIM_REMOVED_CMD"){

            switch (sFields[3].toInt()) {
            case 0:
                ui->lineEdit_REMOVED_TraimEnabled->setText("0 = OFF");
                break;
            case 1:
                ui->lineEdit_REMOVED_TraimEnabled->setText("1 = ON");
                break;
            default:
                break;
            }
            ui->lineEdit_REMOVED_RemSats->setText(sFields[4]);
            QString tmp=sFields[5];
            for (int i=6; i<sFields.count(); i++) {
                tmp.append(",");
                tmp.append(sFields[i]);
            }
            ui->lineEdit_REMOVED_SatN->setText(tmp);
        }else if(tmp_L26DR =="page_PSTMODOREQ"){
            ui->lineEdit_PSTMODOREQ_Timestamp->setText(sFields[1]);
            ui->lineEdit_PSTMODOREQ_DateStamp->setText(sFields[2]);
            ui->lineEdit_PSTMODOREQ_Odo_A->setText(sFields[3]);
            ui->lineEdit_PSTMODOREQ_Odo_B->setText(sFields[4]);
            ui->lineEdit_PSTMODOREQ_Odo_Pon->setText(sFields[5]);
        }else if(tmp_L26DR =="page_PSTMGEOFENCEREQ"){
            ui->lineEdit_PSTMGEOFENCEREQ_Timestamp->setText(sFields[1]);
            ui->lineEdit_PSTMGEOFENCEREQ_Datestamp->setText(sFields[2]);
            QString tmp=sFields[3];
            for (int i=6; i<sFields.count(); i++) {
                tmp.append(",");
                tmp.append(sFields[i]);
            }
        }
    }else{
        if(sFields[1]=="1201")
        {
            //Low bit
            tmp_Low = sFields[2];
        }else if(sFields[1] =="1228")
        {
            //High bit
            tmp_High = sFields[2];
        }else if(sFields[1]=="1500")
        {
            if(tmp_L26DR =="page_PSTMGETPAR_1500"){
                ui->lineEdit_VersionInfo->setText(sFields[2]);
            }
        }else if(sFields[1] =="1122")
        {
            if(tmp_L26DR == "page_PSTMGETPAR_1122"){
                ui->lineEdit_PSTMGETPAR_NMEA_Format->setText(sFields[2]=="0x04"? "0x04 = NMEA is 4.10 like":"0x0C = NMEA is 3.01 like");
            }
        }
    }
}

void configuration_L26DR::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    ui->stackedWidget->setCurrentWidget(ui->stackedWidget->findChildren<QWidget *>(current->text(1))[0]);
    //get Low and High
    if(ui->stackedWidget->currentWidget()->objectName()=="page_PSTMCFGMSGL")
    {getLowHighbit();}
}

void configuration_L26DR::on_pushButton_send_clicked()
{
    QByteArray text = (ui->lineEditCommand->text()+"\r\n").toUtf8();
    emit sendData(text.data(), text.size());
}

void configuration_L26DR::on_GC_PSTMINITGPS_clicked()
{
    QByteArray cmd("$PSTMINITGPS,");
    cmd.append(ui->lineEdit_PSTMINITGPS_Lat->text());
    cmd.append(",");
    cmd.append(ui->comboBox_PSTMINITGPS_NS->currentText());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMINITGPS_Lon->text());
    cmd.append(",");
    cmd.append(ui->comboBox_PSTMINITGPS_EW->currentText());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMINITGPS_Alt->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMINITGPS_Day->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMINITGPS_Month->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMINITGPS_Year->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMINITGPS_Hour->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMINITGPS_Minute->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMINITGPS_Second->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMINITTIME_clicked()
{
    QByteArray cmd("$PSTMINITTIME,");
    cmd.append(ui->lineEdit_PSTMINITTIME_Day->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMINITTIME_Month->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMINITTIME_Year->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMINITTIME_Hour->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMINITTIME_Minute->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMINITTIME_Second->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMCLREPHS_clicked()
{
    QByteArray cmd("$PSTMCLREPHS");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMDUMPEPHEMS_clicked()
{
    QByteArray cmd("$PSTMDUMPEPHEMS");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC2_PSTMCLRALMS_clicked()
{
    QByteArray cmd("$PSTMCLRALMS");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC2_PSTMDUMPALMANAC_clicked()
{
    QByteArray cmd("$PSTMDUMPALMANAC");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMCOLD_clicked()
{
    QByteArray cmd("$PSTMCOLD");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMWARM_clicked()
{
    QByteArray cmd("$PSTMWARM");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMHOT_clicked()
{
    QByteArray cmd("$PSTMHOT");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMSRR_clicked()
{
    QByteArray cmd("$PSTMSRR");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMSBASONOFF_clicked()
{
    QByteArray cmd("$PSTMSBASONOFF");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMSBASSERVICE_clicked()
{
    QByteArray cmd("$PSTMSBASSERVICE,");
    switch (ui->comboBox_PSTMSBASSERVICE->currentIndex()) {
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
        cmd.append(QString::number(ui->comboBox_PSTMSBASSERVICE->currentIndex()));
        break;
    case 5:
        cmd.append(QString::number(7));
        break;
    case 6:
        cmd.append(QString::number(15));
        break;
    default:
        break;
    }
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_poll_PSTMGETRTCTIME_clicked()
{
    QByteArray text("$PSTMGETRTCTIME");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}


void configuration_L26DR::on_GC2_PSTMSETCONSTMASK_clicked()
{
    int tmp_Hex = 0;
    tmp_Hex |= ui->checkBox_GPS->isChecked()?     0x1  : 0x0;
    tmp_Hex |= ui->checkBox_GLONASS->isChecked()? 0x2  : 0x0;
    tmp_Hex |= ui->checkBox_QZSS->isChecked()?    0x4  : 0x0;
    tmp_Hex |= ui->checkBox_Galileo->isChecked()? 0x8  : 0x0;
    tmp_Hex |= ui->checkBox_BDS->isChecked()?     0x80 : 0x0;
    QByteArray cmd("$PSTMSETCONSTMASK,");
    cmd.append(QString::number(tmp_Hex));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMCFGCONST_clicked()
{
    QByteArray cmd("$PSTMCFGCONST,");
    cmd.append(QString::number(ui->comboBox_PSTMCFGCONST_GPS->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PSTMCFGCONST_GLONASS->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PSTMCFGCONST_Galileo->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PSTMCFGCONST_QZSS->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PSTMCFGCONST_BDS->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_poll_PPS_IF_PULSE_DATA_CMD_clicked()
{
    QByteArray text("$PSTMPPS,1,7");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_L26DR::on_poll_PPS_IF_TIMING_DATA_CMD_clicked()
{
    QByteArray text("$PSTMPPS,1,12");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_L26DR::on_poll_PPS_POSITION_HOLD_clicked()
{
    QByteArray text("$PSTMPPS,1,13");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_L26DR::on_poll_PPS_IF_TRAIM_CMD_clicked()
{
    QByteArray text("$PSTMPPS,1,15");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_L26DR::on_poll_PPS_IF_TRAIM_USED_CMD_clicked()
{
    QByteArray text("$PSTMPPS,1,16");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_L26DR::on_poll_PPS_IF_TRAIM_RES_CMD_clicked()
{
    QByteArray text("$PSTMPPS,1,17");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_L26DR::on_poll_PPS_IF_TRAIM_REMOVED_CMD_clicked()
{
    QByteArray text("$PSTMPPS,1,18");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_L26DR::on_GC_PPS_IF_ON_OFF_CMD_clicked()
{
    QByteArray cmd("$PSTMPPS,2,1,");
    cmd.append(QString::number(ui->comboBox_ON_OFF_OnOff->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PPS_IF_OUT_MODE_CMD_clicked()
{
    QByteArray cmd("$PSTMPPS,2,2,");
    cmd.append(QString::number(ui->comboBox_OUT_OutMode->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PPS_IF_REFERENCE_TIME_CMD_clicked()
{
    QByteArray cmd("$PSTMPPS,2,19,");
    cmd.append(QString::number(ui->comboBox_REFERENCE_TIME_TextLabel->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PPS_IF_PULSE_DELAY_CMD_clicked()
{
    QByteArray cmd("$PSTMPPS,2,4,");
    cmd.append(ui->lineEdit_PULSE_DELAY_PulseDela->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PPS_IF_CONSTELLATION_RF_DELAY_CMD_clicked()
{
    QByteArray cmd("$PSTMPPS,2,20,");
    QString tmp=QString::number(ui->comboBox_CONSTELLATION_SatType->currentIndex());
    if(ui->comboBox_CONSTELLATION_SatType->currentIndex()==2)
    {  tmp = "3";}
    if(ui->comboBox_CONSTELLATION_SatType->currentIndex()==3)
    {  tmp = "7";}
    cmd.append(tmp);
    cmd.append(",");
    cmd.append(ui->lineEdit_CONSTELLATION_TimeDelay->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PPS_IF_PULSE_DURATION_CMD_clicked()
{
    QByteArray cmd("$PSTMPPS,2,5,");
    cmd.append(ui->lineEdit_DURATION_PulseDuration->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PPS_IF_PULSE_POLARITY_CMD_clicked()
{
    QByteArray cmd("$PSTMPPS,2,6,");
    cmd.append(QString::number(ui->comboBox_PULSE_POLARITY->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_Set_PPS_IF_PULSE_DATA_CMD_clicked()
{
    QByteArray cmd("$PSTMPPS,2,7,");
    cmd.append(QString::number(ui->comboBox_Set_PULSE_OutMode->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox__Set_PULSE_ReferenceTime->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_Set_PULSE_PulseDelay->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_Set_PULSE_PulseDuration->text());
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_Set_PULSE_PulsePolarity->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PPS_IF_FIX_CONDITION_CMD_clicked()
{
    QByteArray cmd("$PSTMPPS,2,8,");
     cmd.append(QString::number(ui->comboBox__FIX_CONDITION_TextLabel->currentIndex()+1));
     addNMEA_MC_CS(cmd);
     ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PPS_IF_SAT_TRHESHOLD_CMD_clicked()
{
    QByteArray cmd("$PSTMPPS,2,9,");
    cmd.append(ui->lineEdit_PPS_SAT_SatTh->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PPS_IF_ELEVATION_MASK_CMD_clicked()
{
    QByteArray cmd("$PSTMPPS,2,10,");
    cmd.append(ui->lineEdit_ELEVATION->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PPS_IF_CONSTELLATION_MASK_CMD_clicked()
{
    int tmp_Hex = 0;
    tmp_Hex |= ui->checkBox_CONSTELLATION_GPS->isChecked()?     0x1  : 0x0;
    tmp_Hex |= ui->checkBox_CONSTELLATION_GLONASS->isChecked()? 0x2  : 0x0;
    tmp_Hex |= ui->checkBox_CONSTELLATION_Galileo->isChecked()? 0x8  : 0x0;
    tmp_Hex |= ui->checkBox_CONSTELLATION_BDS->isChecked()?     0x80 : 0x0;

    QByteArray cmd("$PSTMPPS,2,11,");
    cmd.append(QString::number(tmp_Hex));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_Setting_PPS_IF_TIMING_DATA_CMD_clicked()
{
    int tmp_Hex = 0;
    tmp_Hex |= ui->checkBox_SET_TIMING_GPS->isChecked()?     0x1 : 0x0;
    tmp_Hex |= ui->checkBox_SET_TIMING_GLONASS->isChecked()? 0x2 : 0x0;
    tmp_Hex |= ui->checkBox_SET_TIMING_Galileo->isChecked()? 0x8 : 0x0;
    tmp_Hex |= ui->checkBox_SET_TIMING_BDS->isChecked()?     0x80: 0x0;

    QByteArray cmd("$PSTMPPS,2,12,");
    cmd.append(QString::number(ui->comboBox_SET_TIMING_FixCondition->currentIndex()+1));
    cmd.append(",");
    cmd.append(ui->lineEdit_SET_TIMING_SatTh->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_SET_TIMING_ElevationMask->text());
    cmd.append(",");
    cmd.append(QString::number(tmp_Hex));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_Set_PPS_IF_POSITION_HOLD_DATA_CMD_clicked()
{
    QByteArray cmd("$PSTMPPS,2,13,");
    cmd.append(QString::number(ui->comboBox_SET_HOLD_OnOff->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_SET_HOLD_Lat->text());
    cmd.append(",");
    cmd.append(ui->comboBox_SET_HOLD_NS->currentIndex()==0? "N":"S");
    cmd.append(",");
    cmd.append(ui->lineEdit_SET_HOLD_Lon->text());
    cmd.append(",");
    cmd.append(ui->comboBox_SET_HOLD_EW->currentIndex()==0? "E":"W");
    cmd.append(",");
    cmd.append(ui->lineEdit_SET_HOLD_H_Msl->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PPS_IF_AUTO_HOLD_SAMPLES_CMD_clicked()
{
    QByteArray cmd("$PSTMPPS,2,14,");
    cmd.append(ui->lineEdit_Auto_PH_Samples->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_Setting_PPS_IF_TRAIM_CMD_clicked()
{
    QByteArray cmd("$PSTMPPS,2,15,");
    cmd.append(QString::number(ui->comboBox_SET_TRAIM_OnOff->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_SET_TRAIM_Alarm->text());
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_SET_TRAIM_AdaptiveMode->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMFORCESTANDBY_clicked()
{
    QByteArray cmd("$PSTMFORCESTANDBY,");
    cmd.append(ui->lineEdit_PSTMFORCESTANDBY->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}
//L26-DR (UDR/ADR/ADRC) and L26-P do not support this command.
void configuration_L26DR::on_poll_PSTMGEOFENCEREQ_clicked()
{
    QByteArray text("$PSTMGEOFENCEREQ");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}
//L26-DR (UDR/ADRC) module does not support this command.
void configuration_L26DR::on_GC_PSTMODOSTART_clicked()
{
    QByteArray cmd("$PSTMODOSTART");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}
//L26-DR (UDR/ADRC) module does not support this command
void configuration_L26DR::on_GC_PSTMODOSTOP_clicked()
{
    QByteArray cmd("$PSTMODOSTOP");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}
//L26-DR (UDR/ADRC) module does not support this command.
void configuration_L26DR::on_poll_PSTMODOREQ_clicked()
{
    QByteArray text("$PSTMODOREQ");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_L26DR::on_GC_PSTMODORESET_clicked()
{
    QByteArray cmd("$PSTMODORESET,");
    cmd.append(QString::number(ui->comboBox_PSTMODORESET->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMCFGPORT_clicked()
{
    QByteArray cmd("$PSTMCFGPORT,");
    cmd.append(QString::number(ui->comboBox_PSTMCFGPORT_PortType->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PSTMCFGPORT_ProtocolType->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMCFGPORT_PortNumb->text());
    cmd.append(",");
    cmd.append(ui->comboBox_PSTMCFGPORT_Baudrate->currentText());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMCFGTDATA_clicked()
{
    QByteArray cmd("$PSTMCFGTDATA,");
    cmd.append(ui->lineEdit_PSTMCFGTDATA_GPS_Min_Week->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMCFGTDATA_GPS_Max_Week->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMCFGTDATA_FixRate->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMCFGTDATA_UTC_Delta->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::getLowHighbit()
{
    QByteArray text("$PSTMGETPAR,1201*21\r\n");
    emit sendData(text.data(), text.size());
    QByteArray text1("$PSTMGETPAR,1228*2A\r\n");
    emit sendData(text1.data(), text1.size());
}

void configuration_L26DR::on_GC_PSTMCFGMSGL_clicked()
{
    bool ok;
    long long tmp_hexLow = tmp_Low.toInt(&ok,16);
    long long tmp_hexHigh = tmp_High.toInt(&ok,16);
    QByteArray cmd("$PSTMCFGMSGL,");
    cmd.append(QString::number(ui->comboBox_PSTMCFGMSGL_ListID->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMCFGMSGL_Rate->text());
    cmd.append(",");
    //Low
    tmp_hexLow = ui->checkBox_GNS->isChecked()?( tmp_hexLow |= 0x1) : (tmp_hexLow &= (~0x1));
    tmp_hexLow = ui->checkBox_GGA->isChecked()?( tmp_hexLow |= 0x2) : (tmp_hexLow &= (~0x2));
    tmp_hexLow = ui->checkBox_GSA->isChecked()?( tmp_hexLow |= 0x4) : (tmp_hexLow &= (~0x4));
    tmp_hexLow = ui->checkBox_GST->isChecked()?( tmp_hexLow |= 0x8) : (tmp_hexLow &= (~0x8));
    tmp_hexLow = ui->checkBox_VTG->isChecked()?( tmp_hexLow |= 0x10) : (tmp_hexLow &= (~0x10));
    tmp_hexLow = ui->checkBox_RMC->isChecked()?( tmp_hexLow |= 0x40) : (tmp_hexLow &= (~0x40));
    tmp_hexLow = ui->checkBox_PSTMTG->isChecked()?( tmp_hexLow |= 0x100) : (tmp_hexLow &= (~0x100));
    tmp_hexLow = ui->checkBox_PSTMTS->isChecked()?( tmp_hexLow |= 0x200) : (tmp_hexLow &= (~0x200));
    tmp_hexLow = ui->checkBox_PSTMSBAS->isChecked()?( tmp_hexLow |= 0x20000) : (tmp_hexLow &= (~0x20000));
    tmp_hexLow = ui->checkBox_GSV->isChecked()?( tmp_hexLow |= 0x80000) : (tmp_hexLow &= (~0x80000));
    tmp_hexLow = ui->checkBox_GLL->isChecked()?( tmp_hexLow |= 0x100000) : (tmp_hexLow &= (~0x100000));
    tmp_hexLow = ui->checkBox_PSTMPPSDATA->isChecked()?( tmp_hexLow |= 0x200000) : (tmp_hexLow &= (~0x200000));
    tmp_hexLow = ui->checkBox_PSTMCPU->isChecked()?( tmp_hexLow |= 0x800000) : (tmp_hexLow &= (~0x800000));
    tmp_hexLow = ui->checkBox_ZDA->isChecked()?( tmp_hexLow |= 0x1000000) : (tmp_hexLow &= (~0x1000000));
    tmp_hexLow = ui->checkBox_PSTMNOTCHSTATUS->isChecked()?( tmp_hexLow |= 0x40000000) : (tmp_hexLow &= (~0x40000000));
    cmd.append(QString("%1").arg(tmp_hexLow,8,16,QLatin1Char('0')));
    cmd.append(",");
    //High
    tmp_hexHigh = ui->checkBox_PSTMUTC->isChecked()? (tmp_hexHigh |= 0x4) : (tmp_hexHigh &= (~0x4));
    tmp_hexHigh = ui->checkBox_PSTMANTENNASTATUS->isChecked()? (tmp_hexHigh |= 0x10) : (tmp_hexHigh &= (~0x10));
    tmp_hexHigh = ui->checkBox_GBS->isChecked()? (tmp_hexHigh |= 0x2000) : (tmp_hexHigh &= (~0x2000));
    tmp_hexHigh = ui->checkBox_PSTMODO->isChecked()? (tmp_hexHigh |= 0x40000) : (tmp_hexHigh &= (~0x40000));
    tmp_hexHigh = ui->checkBox_PSTMGEOFENCESTATUS->isChecked()? (tmp_hexHigh |= 0x80000) : (tmp_hexHigh &= (~0x80000));
    cmd.append(QString::number(tmp_hexHigh,16));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
    getLowHighbit();
}

void configuration_L26DR::on_GC_PSTMCFGAGPS_clicked()
{
    QByteArray cmd("$PSTMCFGAGPS,");
    cmd.append(QString::number(ui->comboBox_PSTMCFGAGPS->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMCFGAJM_clicked()
{
    QByteArray cmd("$PSTMCFGAJM,");
    cmd.append(QString::number(ui->comboBox_PSTMCFGAJM_GPS->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PSTMCFGAJM_GLONASS->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}
//L26-DR (UDR/ADRC) module does not support this command.
void configuration_L26DR::on_GC_PSTMCFGODO_clicked()
{
    QByteArray cmd("$PSTMCFGODO,");
    cmd.append(QString::number(ui->comboBox_PSTMCFGODO_En->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PSTMCFGODO_Auto->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PSTMCFGODO_Enmsg->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMCFGODO_Alarm->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}
//L26-DR (UDR/ADR/ADRC) module does not support this command.
void configuration_L26DR::on_GC_PSTMCFGGEOFENCE_clicked()
{
    QByteArray cmd("$PSTMCFGGEOFENCE,");
    cmd.append(QString::number(ui->comboBox_PSTMCFGGEOFENCE_En->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PSTMCFGGEOFENCE_Tol->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}
//Only L26-DR (UDR/ADR/ADRC) and L26-P support this command.
void configuration_L26DR::on_GC_PSTMIMUSELFTESTCMD_clicked()
{
    QByteArray cmd("$PSTMIMUSELFTESTCMD,");
    cmd.append(QString::number(ui->comboBox_PSTMIMUSELFTESTCMD_IMU_Cat->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}
//Only L26-P module supports this command.
void configuration_L26DR::on_GC_PSTMCFGGEOCIR_clicked()
{
    QByteArray cmd("$PSTMCFGGEOCIR,");
    cmd.append(ui->lineEdit_PSTMCFGGEOCIR_Circleid->text());
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PSTMCFGGEOCIR->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMCFGGEOCIR_Lat->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMCFGGEOCIR_Lon->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMCFGGEOCIR_Rad->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMSETTHTRK_clicked()
{
    QByteArray cmd("$PSTMSETTHTRK,");
    cmd.append(ui->lineEdit_PSTMSETTHTRK_C_No->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMSETTHTRK_El->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMSETTHPOS_clicked()
{
    QByteArray cmd("$PSTMSETTHPOS,");
    cmd.append(ui->lineEdit_PSTMSETTHPOS_C_No->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PSTMSETTHPOS_El->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_poll_PSTMGETPAR_1500_clicked()
{
    QByteArray text("$PSTMGETPAR,1500");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_L26DR::on_GC_PSTMSETPAR_1122_clicked()
{
    QByteArray cmd("$PSTMSETPAR,1122,");
    QString tmp;
    switch (ui->comboBox_PSTMSETPAR_NMEA_Format->currentIndex()) {
    case 0:
        tmp = "0x01";
        break;
    case 1:
        tmp = "0x02";
        break;
    case 2:
        tmp = "0x04";
        break;
    case 3:
        tmp = "0x0C";
        break;
    default:
        break;
    }
    cmd.append(tmp);
    cmd.append(",1");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_poll_PSTMGETPAR_1122_clicked()
{
    QByteArray text("$PSTMGETPAR,1122");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_L26DR::on_GC_PSTMSAVEPAR_clicked()
{
    QByteArray cmd("$PSTMSAVEPAR");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_L26DR::on_GC_PSTMRESTOREPAR_clicked()
{
    QByteArray cmd("$PSTMRESTOREPAR");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

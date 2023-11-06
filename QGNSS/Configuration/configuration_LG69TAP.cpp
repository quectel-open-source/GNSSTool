#include "configuration_LG69TAP.h"
#include "ui_configuration_LG69TAP.h"
#include <QtDebug>
configuration_LG69TAP::configuration_LG69TAP(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configuration_LG69TAP)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->treeWidget->setColumnWidth(0,400);
    this->setAttribute(Qt::WA_QuitOnClose,false);
    this->setWindowTitle("Configuration:LG69TAP");
    ui->treeWidget->hideColumn(1);
}

configuration_LG69TAP::~configuration_LG69TAP()
{
    delete ui;
}

void configuration_LG69TAP::updateUI(NMEA_t *msg)
{
    QString tmp_LG69T_AP = ui->stackedWidget->currentWidget()->objectName();

    if((tmp_LG69T_AP == "page_PQTMCFGPORT")&&(QString::fromUtf8(msg->LG69TAP.commandName) == "$PQTMPORT")){
        ui->comboBox_PortType->setCurrentIndex(msg->LG69TAP.nPortType);
        (msg->LG69TAP.nProtocolType&0x1)!=0 ? ui->checkBox_NMEAInput->setCheckState(Qt::Checked):ui->checkBox_NMEAInput->setCheckState(Qt::Unchecked);
        ((msg->LG69TAP.nProtocolType>>1)&0x1)!=0 ? ui->checkBox_NMEAOutput->setCheckState(Qt::Checked):ui->checkBox_NMEAOutput->setCheckState(Qt::Unchecked);
        ((msg->LG69TAP.nProtocolType>>2)&0x1)!=0 ? ui->checkBox_RTCMOutput->setCheckState(Qt::Checked):ui->checkBox_RTCMOutput->setCheckState(Qt::Unchecked);
        ((msg->LG69TAP.nProtocolType>>3)&0x1)!=0 ? ui->checkBox_RTCMInput->setCheckState(Qt::Checked):ui->checkBox_RTCMInput->setCheckState(Qt::Unchecked);
        ((msg->LG69TAP.nProtocolType>>4)&0x1)!=0 ? ui->checkBox_DebugOutput->setCheckState(Qt::Checked):ui->checkBox_DebugOutput->setCheckState(Qt::Unchecked);
        switch (msg->LG69TAP.nBaudRate) {
        case 115200:
            ui->comboBox_BaudRate->setCurrentIndex(0);
            break;
        case 230400:
            ui->comboBox_BaudRate->setCurrentIndex(1);
            break;
        case 460800:
            ui->comboBox_BaudRate->setCurrentIndex(2);
            break;
        case 921600:
            ui->comboBox_BaudRate->setCurrentIndex(3);
            break;
        default:
            break;
        }

    }else if((tmp_LG69T_AP == "page_PQTMSET_GETNMEAMSGMASK")&&(QString::fromUtf8(msg->LG69TAP.commandName) == "$PQTMNMEAMSGMASK")){
        ((msg->LG69TAP.MsgMask>>30)&0x1) !=0 ? ui->checkBox_Vehicle->setCheckState(Qt::Checked):ui->checkBox_Vehicle->setCheckState(Qt::Unchecked);
        ((msg->LG69TAP.MsgMask>>31)&0x1) !=0 ? ui->checkBox_Sensor->setCheckState(Qt::Checked):ui->checkBox_Sensor->setCheckState(Qt::Unchecked);

    }else if((tmp_LG69T_AP == "page_PQTMCFGDRMODE")&&(QString::fromUtf8(msg->LG69TAP.commandName) == "$PQTMDRMODE")){
        ui->comboBox_PQTMCFGDRMODE_SpeedMode->setCurrentIndex(msg->LG69TAP.nSpeedMode);
    }else if((tmp_LG69T_AP == "page_PQTMCFGFWD")&&(QString::fromUtf8(msg->LG69TAP.commandName) == "$PQTMFWD")){
        ui->comboBox_PQTMCFGFWD_Mode->setCurrentIndex(msg->LG69TAP.nMode);
        ui->comboBox_PQTMCFGFWD_Invert->setCurrentIndex(msg->LG69TAP.nInvert);
        ui->comboBox_PQTMCFGFWD_Pull->setCurrentIndex(msg->LG69TAP.nPull);

    }else if((tmp_LG69T_AP == "page_PQTMCFGWHEELTICK")&&(QString::fromUtf8(msg->LG69TAP.commandName) == "$PQTMWHEELTICK")){
        ui->comboBox_PQTMCFGWHEELTICK_EdgeType->setCurrentIndex(msg->LG69TAP.nEdgeType);
        ui->comboBox_PQTMCFGWHEELTICK_Pull->setCurrentIndex(msg->LG69TAP.nPull);
        ui->lineEdit_PQTMCFGWHEELTICK_MPT->setText(QString::number(msg->LG69TAP.dMPT,'f'));

    }else if((tmp_LG69T_AP == "page_PQTMCFGCAN")&&(QString::fromUtf8(msg->LG69TAP.commandName) == "$PQTMCAN")){
        ui->comboBox_PQTMCFGCAN_PortID->setCurrentIndex(msg->LG69TAP.nPortID);
        ui->comboBox_PQTMCFGCAN_Enable->setCurrentIndex(msg->LG69TAP.nEnable);
        ui->comboBox_PQTMCFGCAN_FrameFormat->setCurrentIndex(msg->LG69TAP.nFrameFormat);
        ui->lineEdit_PQTMCFGCAN_Baudrate->setText(QString::number(msg->LG69TAP.nBaudRate));
        ui->lineEdit_PQTMCFGCAN_DataBaudrate->setText(QString::number(msg->LG69TAP.nDataBaudrate));

    }else if((tmp_LG69T_AP == "page_PQTMCFGCANFILTER")&&(QString::fromUtf8(msg->LG69TAP.commandName) == "$PQTMCANFILTER")){
        ui->comboBox_PQTMCFGCANFILTER_PortID->setCurrentIndex(msg->LG69TAP.nPortID);
        ui->comboBox_PQTMCFGCANFILTER_Index->setCurrentIndex(msg->LG69TAP.nIndex);
        ui->comboBox_PQTMCFGCANFILTER_Enable->setCurrentIndex(msg->LG69TAP.nEnable);
        ui->comboBox_PQTMCFGCANFILTER_FilterType->setCurrentIndex(msg->LG69TAP.nFilterType);
        ui->comboBox_PQTMCFGCANFILTER_ID_Type->setCurrentIndex(msg->LG69TAP.nID_Type);
        ui->lineEdit_PQTMCFGCANFILTER_ID1->setText(QString::fromUtf8(msg->LG69TAP.ID1));
        ui->lineEdit_PQTMCFGCANFILTER_ID2->setText(QString::fromUtf8(msg->LG69TAP.ID2));

    }else if((tmp_LG69T_AP == "page_PQTMCFGVEHDBC")&&(QString::fromUtf8(msg->LG69TAP.commandName) == "$PQTMVEHDBC")){
        ui->comboBox_PQTMCFGVEHDBC_Index->setCurrentIndex(msg->LG69TAP.nIndex);
        ui->lineEdit_PQTMCFGVEHDBC_MsgID->setText(QString::fromUtf8(msg->LG69TAP.MsgID));
        ui->lineEdit_PQTMCFGVEHDBC_StartBit->setText(QString::number(msg->LG69TAP.nStartBit));
        ui->lineEdit_PQTMCFGVEHDBC_BitSize->setText(QString::number(msg->LG69TAP.nBitSize));
        ui->comboBox_PQTMCFGVEHDBC_ByteOrder->setCurrentIndex(msg->LG69TAP.nByteOrder);
        ui->lineEdit_PQTMCFGVEHDBC_ValueType->setText(QString::number(msg->LG69TAP.nValueType));
        ui->lineEdit_PQTMCFGVEHDBC_Factor->setText(QString::number(msg->LG69TAP.dFactor,'f'));
        ui->lineEdit_PQTMCFGVEHDBC_Offset->setText(QString::number(msg->LG69TAP.dOffset,'f'));
        ui->lineEdit_PQTMCFGVEHDBC_Min->setText(QString::number(msg->LG69TAP.dMin,'f'));
        ui->lineEdit_PQTMCFGVEHDBC_Max->setText(QString::number(msg->LG69TAP.dMax,'f'));

    }else if((tmp_LG69T_AP == "page_PQTMSET_GETVEHRVAL")&&(QString::fromUtf8(msg->LG69TAP.commandName) == "$PQTMVEHRVAL")){
        ui->lineEdit_PQTMSET_GETVEHRVAL->setText(QString::number(msg->LG69TAP.nRVal));

    }else if((tmp_LG69T_AP == "page_PQTMVERNO")&&(QString::fromUtf8(msg->LG69TAP.commandName) == "$PQTMVERNO")){
        ui->lineEdit_MainVersion->setText(QString::fromUtf8(msg->LG69TAP.mainVersion));
    }

}

void configuration_LG69TAP::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    ui->stackedWidget->setCurrentWidget(ui->stackedWidget->findChildren<QWidget *>(current->text(1))[0]);
}

void configuration_LG69TAP::on_pushButton_send_clicked()
{
    QByteArray text = (ui->lineEditCommand->text()+"\r\n").toUtf8();
    emit sendData(text.data(), text.size());
    qDebug()<<" QByteArray_LG69TAP:"<<text;
}

void configuration_LG69TAP::on_GC_PQTMCOLD_clicked()
{
    QByteArray cmd("$PQTMCOLD");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}


void configuration_LG69TAP::on_GC_PQTMWARM_clicked()
{
    QByteArray cmd("$PQTMWARM");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_GC_PQTMHOT_clicked()
{
    QByteArray cmd("$PQTMHOT");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_GC_PQTMSRR_clicked()
{
    QByteArray cmd("$PQTMSRR");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_GC_PQTMCFGPORT_clicked()
{
    int tmpBit=0;QString tmp;
    QByteArray cmd("$PQTMCFGPORT,1,");
    cmd.append(QString::number(ui->comboBox_PortType->currentIndex()));
    cmd.append(",");
    tmpBit |= ui->checkBox_NMEAInput->isChecked()?   0x1 :0x0;
    tmpBit |= ui->checkBox_NMEAOutput->isChecked()?  0x2 :0x0;
    tmpBit |= ui->checkBox_RTCMOutput->isChecked()?  0x4 :0x0;
    tmpBit |= ui->checkBox_RTCMInput->isChecked()?   0x8 :0x0;
    tmpBit |= ui->checkBox_DebugOutput->isChecked()? 0x10:0x0;
    cmd.append(QString::number(tmpBit));
    cmd.append(",");
    switch (ui->comboBox_BaudRate->currentIndex()) {
    case 0:
        tmp = "115200";
        break;
    case 1:
        tmp = "230400";
        break;
    case 2:
        tmp = "460800";
        break;
    case 3:
        tmp = "921600";
        break;
    default:
        break;
    }
    cmd.append(tmp);
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_poll_PQTMCFGPORT_clicked()
{
    QByteArray text("$PQTMCFGPORT,0,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LG69TAP::on_GC_PQTMGNSSSUSPEND_clicked()
{
    QByteArray cmd("$PQTMGNSSSUSPEND");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_GC_PQTMGNSSRESUME_clicked()
{
    QByteArray cmd("$PQTMGNSSRESUME");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_GC_PQTMGNSSRESTART_clicked()
{
    QByteArray cmd("$PQTMGNSSRESTART");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_GC_PQTMSET_GETNMEAMSGMASK_clicked()
{
    QByteArray cmd("$PQTMSETNMEAMSGMASK,");
    uint tmp_hex=0;
    tmp_hex |= ui->checkBox_Vehicle->isChecked()? 0x40000000:0x0;
    tmp_hex |= ui->checkBox_Sensor->isChecked()?  0x80000000:0x0;
    cmd.append("0x"+QString::number(tmp_hex,16));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_poll_PQTMSET_GETNMEAMSGMASK_clicked()
{
    QByteArray text("$PQTMGETNMEAMSGMASK");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LG69TAP::on_GC_PQTMCFGDRMODE_clicked()
{
    QByteArray cmd("$PQTMCFGDRMODE,1,");
    cmd.append(QString::number(ui->comboBox_PQTMCFGDRMODE_SpeedMode->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_poll_PQTMCFGDRMODE_clicked()
{
    QByteArray text("$PQTMCFGDRMODE,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LG69TAP::on_GC_PQTMCFGFWD_clicked()
{
    QByteArray cmd("$PQTMCFGFWD,1,");
    cmd.append(QString::number(ui->comboBox_PQTMCFGFWD_Mode->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGFWD_Invert->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGFWD_Pull->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_poll_PQTMCFGFWD_clicked()
{
    QByteArray text("$PQTMCFGFWD,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LG69TAP::on_GC_PQTMCFGWHEELTICK_clicked()
{
    QByteArray cmd("$PQTMCFGWHEELTICK,1,");
    cmd.append(QString::number(ui->comboBox_PQTMCFGWHEELTICK_EdgeType->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGWHEELTICK_Pull->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGWHEELTICK_MPT->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_poll_PQTMCFGWHEELTICK_clicked()
{
    QByteArray text("$PQTMCFGWHEELTICK,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LG69TAP::on_GC_PQTMCFGCAN_clicked()
{
    QByteArray cmd("$PQTMCFGCAN,1,");
    cmd.append(QString::number(ui->comboBox_PQTMCFGCAN_PortID->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGCAN_Enable->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGCAN_FrameFormat->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGCAN_Baudrate->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGCAN_DataBaudrate->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_poll_PQTMCFGCAN_clicked()
{
    QByteArray text("$PQTMCFGCAN,0,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LG69TAP::on_GC_PQTMCFGCANFILTER_clicked()
{
    QByteArray cmd("$PQTMCFGCANFILTER,1,");
    cmd.append(QString::number(ui->comboBox_PQTMCFGCANFILTER_PortID->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGCANFILTER_Index->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGCANFILTER_Enable->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGCANFILTER_FilterType->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGCANFILTER_ID_Type->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGCANFILTER_ID1->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGCANFILTER_ID2->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_poll_PQTMCFGCANFILTER_clicked()
{
    QByteArray text("$PQTMCFGCANFILTER,0,0,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LG69TAP::on_GC_PQTMCFGVEHDBC_clicked()
{
    QByteArray cmd("$PQTMCFGVEHDBC,1,");
    cmd.append(QString::number(ui->comboBox_PQTMCFGVEHDBC_Index->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGVEHDBC_MsgID->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGVEHDBC_StartBit->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGVEHDBC_BitSize->text());
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGVEHDBC_ByteOrder->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGVEHDBC_ValueType->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGVEHDBC_Factor->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGVEHDBC_Offset->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGVEHDBC_Min->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGVEHDBC_Max->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_poll_PQTMCFGVEHDBC_clicked()
{
    QByteArray text("$PQTMCFGVEHDBC,0,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LG69TAP::on_GC_PQTMSET_GETVEHRVAL_clicked()
{
    QByteArray cmd("$PQTMSETVEHRVAL,");
    cmd.append(ui->lineEdit_PQTMSET_GETVEHRVAL->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_poll_PQTMSET_GETVEHRVAL_clicked()
{
    QByteArray text("$PQTMGETVEHRVAL");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LG69TAP::on_GC_PQTMSAVEPAR_clicked()
{
    QByteArray cmd("$PQTMSAVEPAR");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_GC_PQTMRESTOREPAR_clicked()
{
    QByteArray cmd("$PQTMRESTOREPAR");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAP::on_poll_PQTMVERNO_clicked()
{
    QByteArray text("$PQTMVERNO");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LG69TAP::on_checkBox_RTCMOutput_clicked()
{
    if(ui->checkBox_RTCMOutput->isChecked())
    {
        ui->checkBox_DebugOutput->setCheckState(Qt::Unchecked);
    }
}

void configuration_LG69TAP::on_checkBox_DebugOutput_clicked()
{
    if(ui->checkBox_DebugOutput->isChecked())
    {
        ui->checkBox_RTCMOutput->setCheckState(Qt::Unchecked);
    }
}

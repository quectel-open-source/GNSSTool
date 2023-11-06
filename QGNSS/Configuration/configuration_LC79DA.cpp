#include "configuration_LC79DA.h"
#include "ui_configuration_LC79DA.h"
#include <QDebug>

configuration_LC79DA::configuration_LC79DA(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configuration_LC79DA)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->treeWidget->setColumnWidth(0,400);
    this->setAttribute(Qt::WA_QuitOnClose,false);
    this->setWindowTitle("Configuration:LC79DA");
    ui->treeWidget->hideColumn(1);
}

configuration_LC79DA::~configuration_LC79DA()
{
    delete ui;
}

void configuration_LC79DA::updateUI(NMEA_t* msg)
{
    QString tmp_Lc79D_A = ui->stackedWidget->currentWidget()->objectName();

    if((tmp_Lc79D_A == "page_PQGETSLEEP")&&(QString::fromUtf8(msg->Lc79DA.commandName) == "$PQSLEEP")){
        ui->lineEdit_PQGETSLEEP->setText(msg->Lc79DA.cSleepMode !=0 ? "1 = The sleep mode is enabled":"0 = The sleep mode is disabled");

    }else if((tmp_Lc79D_A == "page_PQGETGLP")&&(QString::fromUtf8(msg->Lc79DA.commandName) == "$PQGLP")){
        ui->lineEdit_PQGETGLP->setText(msg->Lc79DA.cGLPModeStatus !=0 ? "1 = The GLP mode is enabled":"0 = The GLP mode is disabled");

    }else if((tmp_Lc79D_A == "page_PQGETCNST")&&(QString::fromUtf8(msg->Lc79DA.commandName) == "$PQCNST")){
        ui->lineEdit_PQGETCNST_Date_GPS->setText(msg->Lc79DA.c_B0 != 0? "GPS constellation enabling":"GPS constellation disabling");
        ui->lineEdit_PQGETCNST_Date_GLONASS->setText(msg->Lc79DA.c_B1 != 0? "GLONASS constellation enabling":"GLONASS constellation disabling");
        ui->lineEdit_PQGETCNST_Date_Galileo->setText(msg->Lc79DA.c_B2 != 0? "Galileo constellation enabling":"Galileo constellation disabling");
        ui->lineEdit_PQGETCNST_Date_BDS->setText(msg->Lc79DA.c_B3 != 0? "BDS constellation enabling":"BDS constellation disabling");
        ui->lineEdit_PQGETCNST_Date_NavIC->setText(msg->Lc79DA.c_B4 != 0? "NavIC constellation enabling":"NavIC constellation disabling");
        ui->lineEdit_PQGETCNST_Date_QZSS->setText(msg->Lc79DA.c_B5 != 0? "QZSS constellation enabling":"QZSS constellation disabling");

    }else if((tmp_Lc79D_A == "page_PQGETL5BIAS")&&(QString::fromUtf8(msg->Lc79DA.commandName) == "$PQL5BIAS")){
        ui->lineEdit_PQGETL5BIAS->setText(QString::number(msg->Lc79DA.dL5L5Bias,'f'));

    }else if((tmp_Lc79D_A == "page_PQCFGODO")&&(QString::fromUtf8(msg->Lc79DA.commandName) == "$PQCFGODO")){
        ui->comboBox_PQCFGODO_Auto_Start->setCurrentIndex(msg->Lc79DA.nAuto_Start);
        ui->comboBox_PQCFGODO_MSG_Mode->setCurrentIndex(msg->Lc79DA.nMSG_Mode);
        ui->lineEdit_PQCFGODO_InitVal->setText(QString::number(msg->Lc79DA.dInitVal,10,1));

    }else if((tmp_Lc79D_A == "page_PQREQODO")&&(QString::fromUtf8(msg->Lc79DA.commandName) == "$PQODO")){
        ui->lineEdit_PQREQODO->setText(QString::number(msg->Lc79DA.dOdo_Val,10,1));

    }else if((tmp_Lc79D_A == "page_PQCFGGEOFENCE")&&(QString::fromUtf8(msg->Lc79DA.commandName) == "$PQCFGGEOFENCE")){
        ui->lineEdit_PQCFGGEOFENCE_GeoID->setText(QString::number(msg->Lc79DA.nGeoID));
        ui->comboBox_PQCFGGEOFENCE_GeoMode->setCurrentIndex(msg->Lc79DA.nGeoMode);
        ui->comboBox_PQCFGGEOFENCE_GeoShape->setCurrentIndex(msg->Lc79DA.nGeoShape);
        ui->lineEdit_PQCFGGEOFENCE_Lat0->setText(QString::number(msg->Lc79DA.fLat0,'f',4));
        ui->lineEdit_PQCFGGEOFENCE_Lon0->setText(QString::number(msg->Lc79DA.fLon0,'f',4));
        ui->lineEdit_PQCFGGEOFENCE_Lat1_Radius->setText(QString::number(msg->Lc79DA.fLat1,'f',4));
        ui->lineEdit_PQCFGGEOFENCE_Lon1->setText(msg->Lc79DA.fLon1 > 360? "":QString::number(msg->Lc79DA.fLon1,'f',4));
        ui->lineEdit_PQCFGGEOFENCE_Lat2->setText(msg->Lc79DA.fLat2 > 360? "":QString::number(msg->Lc79DA.fLat2,'f',4));
        ui->lineEdit_PQCFGGEOFENCE_Lon2->setText(msg->Lc79DA.fLon2 > 360? "":QString::number(msg->Lc79DA.fLon2,'f',4));
        ui->lineEdit_PQCFGGEOFENCE_Lat3->setText(msg->Lc79DA.fLat3 > 360? "":QString::number(msg->Lc79DA.fLat3,'f',4));
        ui->lineEdit_PQCFGGEOFENCE_Lon3->setText(msg->Lc79DA.fLon3 > 360? "":QString::number(msg->Lc79DA.fLon3,'f',4));

    }else if((tmp_Lc79D_A == "page_PQGETGEOFENCE")&&(QString::fromUtf8(msg->Lc79DA.commandName) == "$PQGEOFENCE")){
        ui->lineEdit_PQGETGEOFENCE->setText(msg->Lc79DA.nGeoEn !=0 ? "1 = Geofence is enabled":"0 = Geofence is disabled");

    }else if((tmp_Lc79D_A == "page_PQCFGNMEAMSG")&&(QString::fromUtf8(msg->Lc79DA.commandName) == "$PQCFGNMEAMSG")){
        ui->comboBox_PQCFGNMEAMSG_NMEA_GGA->setCurrentIndex(msg->Lc79DA.nNMEA_GGA);
        ui->comboBox_PQCFGNMEAMSG_NMEA_RMC->setCurrentIndex(msg->Lc79DA.nNMEA_RMC);
        ui->comboBox_PQCFGNMEAMSG_NMEA_GSV->setCurrentIndex(msg->Lc79DA.nNMEA_GSV);
        ui->comboBox_PQCFGNMEAMSG_NMEA_GSA->setCurrentIndex(msg->Lc79DA.nNMEA_GSA);
        ui->comboBox_PQCFGNMEAMSG_NMEA_GLL->setCurrentIndex(msg->Lc79DA.nNMEA_GLL);
        ui->comboBox_PQCFGNMEAMSG_NMEA_VTG->setCurrentIndex(msg->Lc79DA.nNMEA_VTG);

    }else if((tmp_Lc79D_A == "page_PQCFGEAMASK")&&(QString::fromUtf8(msg->Lc79DA.commandName) == "$PQCFGEAMASK")){
        ui->lineEdit_PQCFGEAMASK->setText(QString::number(msg->Lc79DA.nEA_Mask));

    }else if((tmp_Lc79D_A == "page_PQCFGCLAMPING")&&(QString::fromUtf8(msg->Lc79DA.commandName) == "$PQCFGCLAMPING")){
        ui->comboBox_PQCFGCLAMPING_Enable->setCurrentIndex(msg->Lc79DA.nEnable);
    }
}

void configuration_LC79DA::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    ui->stackedWidget->setCurrentWidget(ui->stackedWidget->findChildren<QWidget *>(current->text(1))[0]);
}

void configuration_LC79DA::on_pushButton_send_clicked()
{
    QByteArray text = (ui->lineEditCommand->text()+"\r\n").toUtf8();
    emit sendData(text.data(), text.size());
    qDebug()<<" QByteArray_LC79DA:"<<text;
}

void configuration_LC79DA::on_GC_PQCOLD_clicked()
{
    QByteArray cmd("$PQCOLD");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_GC_PQWARM_clicked()
{
    QByteArray cmd("$PQWARM");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_GC_PQHOT_clicked()
{
    QByteArray cmd("$PQHOT");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_GC_PQSTARTGNSS_clicked()
{
    QByteArray cmd("$PQSTARTGNSS");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_GC_PQSTOPGNSS_clicked()
{
    QByteArray cmd("$PQSTOPGNSS");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_GC_PQSRR_clicked()
{
    QByteArray cmd("$PQSRR");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_GC_PQSETSLEEP_clicked()
{
    QByteArray cmd("$PQSETSLEEP,");
    cmd.append(QString::number(ui->comboBox_PQSETSLEEP->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_poll_PQGETSLEEP_clicked()
{
    QByteArray text("$PQGETSLEEP");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}
//The GNSS engine will be restarted if this command is executed successfully.
void configuration_LC79DA::on_GC_PQSETGLP_clicked()
{
    QByteArray cmd("$PQSETGLP,");
    cmd.append(QString::number(ui->comboBox_PQSETGLP->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_poll_PQGETGLP_clicked()
{
    QByteArray text("$PQGETGLP");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DA::on_GC_PQSETBAUD_clicked()
{
    QByteArray cmd("$PQSETBAUD,");
    cmd.append(ui->comboBox_PQSETBAUD->currentText());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}
//The GNSS engine will be restarted if the command is executed successfully.
void configuration_LC79DA::on_GC_PQSETCNST_clicked()
{
    int tmp_Hex = 0;
    tmp_Hex |= ui->checkBox_PQSETCNST_GPS->isChecked()?     0x1 :0x0;
    tmp_Hex |= ui->checkBox_PQSETCNST_GLONASS->isChecked()? 0x2 :0x0;
    tmp_Hex |= ui->checkBox_PQSETCNST_Galileo->isChecked()? 0x4 :0x0;
    tmp_Hex |= ui->checkBox_PQSETCNST_BDS->isChecked()?     0x8 :0x0;
    tmp_Hex |= ui->checkBox_PQSETCNST_NavIC->isChecked()?   0x10:0x0;
    tmp_Hex |= ui->checkBox_PQSETCNST_QZSS->isChecked()?    0x20:0x0;

    QByteArray cmd("$PQSETCNST,");
    cmd.append(QString::number(tmp_Hex));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_poll_PQGETCNST_clicked()
{
    QByteArray text("$PQGETCNST");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}
//The configuration takes effect after rebooting, and it must be saved by $PQSAVEPAR (see Chapter2.3.31 for details) before rebooting.
void configuration_LC79DA::on_GC_PQSETL5BIAS_clicked()
{
    QByteArray cmd("$PQSETL5BIAS,");
    cmd.append(ui->lineEdit_PQSETL5BIAS->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_poll_PQGETL5BIAS_clicked()
{
    QByteArray text("$PQGETL5BIAS");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DA::on_GC_PQSETASSTIME_clicked()
{
    QByteArray cmd("$PQSETASSTIME,");
    cmd.append(ui->lineEdit_PQSETASSTIME_Year->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQSETASSTIME_Month->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQSETASSTIME_Day->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQSETASSTIME_Hour->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQSETASSTIME_Min->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQSETASSTIME_Sec->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_GC_PQSETASSPOS_clicked()
{
    QByteArray cmd("$PQSETASSPOS,");
    cmd.append(ui->lineEdit_PQSETASSPOS_Lat->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQSETASSPOS_Lon->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQSETASSPOS_Alt->text());
    if(!ui->lineEdit_PQSETASSPOS_Hor_Acc->text().isEmpty() && !ui->lineEdit_PQSETASSPOS_Ver_Acc->text().isEmpty())
    {
        cmd.append(",");
        cmd.append(ui->lineEdit_PQSETASSPOS_Hor_Acc->text());
        cmd.append(",");
        cmd.append(ui->lineEdit_PQSETASSPOS_Ver_Acc->text());
    }else if(ui->lineEdit_PQSETASSPOS_Hor_Acc->text().isEmpty() && !ui->lineEdit_PQSETASSPOS_Ver_Acc->text().isEmpty())
    {
        cmd.append(",");
        cmd.append(ui->lineEdit_PQSETASSPOS_Ver_Acc->text());
    }else if(ui->lineEdit_PQSETASSPOS_Ver_Acc->text().isEmpty() && !ui->lineEdit_PQSETASSPOS_Hor_Acc->text().isEmpty())
    {
        cmd.append(",");
        cmd.append(ui->lineEdit_PQSETASSPOS_Hor_Acc->text());
    }
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_GC_PQSETASSGNSS_clicked()
{
    QByteArray cmd("$PQSETASSGNSS,");
    cmd.append(ui->lineEdit_PQSETASSGNSS_Lat->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQSETASSGNSS_Lon->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQSETASSGNSS_Alt->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQSETASSGNSS_Year->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQSETASSGNSS_Month->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQSETASSGNSS_Day->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQSETASSGNSS_Hour->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQSETASSGNSS_Min->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQSETASSGNSS_Sec->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_GC_PQCFGODO_clicked()
{
    QByteArray cmd("$PQCFGODO,1,");
    cmd.append(QString::number(ui->comboBox_PQCFGODO_Auto_Start->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQCFGODO_MSG_Mode->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_PQCFGODO_InitVal->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_poll_PQCFGODO_clicked()
{
    QByteArray text("$PQCFGODO,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DA::on_poll_PQREQODO_clicked()
{
    QByteArray text("$PQREQODO");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DA::on_GC_PQRESETODO_clicked()
{
    QByteArray cmd("$PQRESETODO");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}
//返回错误指令,应用的场景问题
void configuration_LC79DA::on_GC_PQSTARTODO_clicked()
{
    QByteArray cmd("$PQSTARTODO");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}
//返回错误指令
void configuration_LC79DA::on_GC_PQSTOPODO_clicked()
{
    QByteArray cmd("$PQSTOPODO");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}
//set成功，get失败，get的指令是什么？是$PQCFGGEOFENCE,0
void configuration_LC79DA::on_GC_PQCFGGEOFENCE_clicked()
{
    QByteArray cmd("$PQCFGGEOFENCE,1,");
    cmd.append(ui->lineEdit_PQCFGGEOFENCE_GeoID->text());
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQCFGGEOFENCE_GeoMode->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQCFGGEOFENCE_GeoShape->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_PQCFGGEOFENCE_Lat0->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQCFGGEOFENCE_Lon0->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQCFGGEOFENCE_Lat1_Radius->text());
    cmd.append(ui->lineEdit_PQCFGGEOFENCE_Lon1->text().isEmpty()? "": ("," + ui->lineEdit_PQCFGGEOFENCE_Lon1->text()));
    cmd.append(ui->lineEdit_PQCFGGEOFENCE_Lat2->text().isEmpty()? "": ("," + ui->lineEdit_PQCFGGEOFENCE_Lat2->text()));
    cmd.append(ui->lineEdit_PQCFGGEOFENCE_Lon2->text().isEmpty()? "": ("," + ui->lineEdit_PQCFGGEOFENCE_Lon2->text()));
    cmd.append(ui->lineEdit_PQCFGGEOFENCE_Lat3->text().isEmpty()? "": ("," + ui->lineEdit_PQCFGGEOFENCE_Lat3->text()));
    cmd.append(ui->lineEdit_PQCFGGEOFENCE_Lon3->text().isEmpty()? "": ("," + ui->lineEdit_PQCFGGEOFENCE_Lon3->text()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_poll_PQCFGGEOFENCE_clicked()
{
    QByteArray text("$PQCFGGEOFENCE,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DA::on_GC_PQSETGEOFENCE_clicked()
{
    QByteArray cmd("$PQSETGEOFENCE");
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQSETGEOFENCE->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_poll_PQGETGEOFENCE_clicked()
{
    QByteArray text("$PQGETGEOFENCE");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}
//发送的指令无问题，但是返回值为错误：$PQREQGEOFENCEERROR*19
void configuration_LC79DA::on_GC_PQREQGEOFENCE_clicked()
{
    QByteArray cmd("$PQREQGEOFENCE");
    cmd.append(",");
    cmd.append(ui->lineEdit_PQREQGEOFENCE->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_GC_PQCFGNMEAMSG_clicked()
{
    QByteArray cmd("$PQCFGNMEAMSG,1,");
    cmd.append(QString::number(ui->comboBox_PQCFGNMEAMSG_NMEA_GGA->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQCFGNMEAMSG_NMEA_RMC->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQCFGNMEAMSG_NMEA_GSV->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQCFGNMEAMSG_NMEA_GSA->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQCFGNMEAMSG_NMEA_GLL->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQCFGNMEAMSG_NMEA_VTG->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);

}

void configuration_LC79DA::on_poll_PQCFGNMEAMSG_clicked()
{
    QByteArray text("$PQCFGNMEAMSG,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DA::on_GC_PQCFGEAMASK_clicked()
{
    QByteArray cmd("$PQCFGEAMASK,1,");
    cmd.append(ui->lineEdit_PQCFGEAMASK->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_poll_PQCFGEAMASK_clicked()
{
    QByteArray text("$PQCFGEAMASK,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DA::on_GC_PQCFGCLAMPING_clicked()
{
    QByteArray cmd("$PQCFGCLAMPING,1,");
    cmd.append(QString::number(ui->comboBox_PQCFGCLAMPING_Enable->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_poll_PQCFGCLAMPING_clicked()
{
    QByteArray text("$PQCFGCLAMPING,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DA::on_GC_PQSAVEPAR_clicked()
{
    QByteArray cmd("$PQSAVEPAR");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DA::on_GC_PQRESTOREPAR_clicked()
{
    QByteArray cmd("$PQRESTOREPAR");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

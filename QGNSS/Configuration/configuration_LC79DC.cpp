#include "configuration_LC79DC.h"
#include "ui_configuration_LC79DC.h"
#include <QDebug>

configuration_LC79DC::configuration_LC79DC(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configuration_LC79DC)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->treeWidget->setColumnWidth(0,400);
    this->setAttribute(Qt::WA_QuitOnClose,false);
    this->setWindowTitle("Configuration:LC79DC");
    ui->treeWidget->hideColumn(1);
}

configuration_LC79DC::~configuration_LC79DC()
{
    delete ui;
}

void configuration_LC79DC::updateUI(NMEA_t *msg)
{
    QString tmp_Lc79D_C = ui->stackedWidget->currentWidget()->objectName();

    if((tmp_Lc79D_C == "page_PQTMSET_GETGLP")&&(QString::fromUtf8(msg->Lc79DC.commandName) == "$PQTMGLP")){
        ui->comboBox_PQTMSET_GETGLP->setCurrentIndex(msg->Lc79DC.nMode);

    }else if((tmp_Lc79D_C == "page_PQTMSET_GETCNST")&&(QString::fromUtf8(msg->Lc79DC.commandName) == "$PQTMCNST")){
        msg->Lc79DC.c_B0 !=0? ui->checkBox_PQTMSET_GETCNST_GPS->setCheckState(Qt::Unchecked):ui->checkBox_PQTMSET_GETCNST_GPS->setCheckState(Qt::Checked);
        msg->Lc79DC.c_B1 !=0? ui->checkBox_PQTMSET_GETCNST_GLONASS->setCheckState(Qt::Unchecked):ui->checkBox_PQTMSET_GETCNST_GLONASS->setCheckState(Qt::Checked);
        msg->Lc79DC.c_B2 !=0? ui->checkBox_PQTMSET_GETCNST_Galileo->setCheckState(Qt::Unchecked):ui->checkBox_PQTMSET_GETCNST_Galileo->setCheckState(Qt::Checked);
        msg->Lc79DC.c_B3 !=0? ui->checkBox_PQTMSET_GETCNST_BDS->setCheckState(Qt::Unchecked):ui->checkBox_PQTMSET_GETCNST_BDS->setCheckState(Qt::Checked);
        msg->Lc79DC.c_B4 !=0? ui->checkBox_PQTMSET_GETCNST_NAVIC->setCheckState(Qt::Unchecked):ui->checkBox_PQTMSET_GETCNST_NAVIC->setCheckState(Qt::Checked);
        msg->Lc79DC.c_B5 !=0? ui->checkBox_PQTMSET_GETCNST_QZSS->setCheckState(Qt::Unchecked):ui->checkBox_PQTMSET_GETCNST_QZSS->setCheckState(Qt::Checked);

    }else if((tmp_Lc79D_C == "page_PQTMSET_GETL5BIAS")&&(QString::fromUtf8(msg->Lc79DC.commandName) == "$PQTML5BIAS")){
        ui->lineEdit_PQTMSET_GETL5BIAS->setText(QString::number(msg->Lc79DC.dL5Bias,'f'));

    }else if((tmp_Lc79D_C == "page_PQTMCFGODO")&&(QString::fromUtf8(msg->Lc79DC.commandName) == "$PQTMCFGODO")){
        ui->comboBox_PQTMCFGODO_Auto_Start->setCurrentIndex(msg->Lc79DC.nAuto_Start);
        ui->comboBox_PQTMCFGODO_MSG_Mode->setCurrentIndex(msg->Lc79DC.nMSG_Mode);
        ui->lineEdit_PQTMCFGODO_InitVal->setText(QString::number(msg->Lc79DC.dInitVal,10,1));

    }else if((tmp_Lc79D_C == "page_PQTMREQODO")&&(QString::fromUtf8(msg->Lc79DC.commandName) == "$PQTMODO")){
        //poll issued a command, but returned PQTMREQODOERROR, the suspected module firmware does not support
        ui->lineEdit_PQTMREQODO->setText(QString::number(msg->Lc79DC.dOdo_Val,10,1));

    }else if((tmp_Lc79D_C == "page_PQTMCFGGEOFENCE")&&(QString::fromUtf8(msg->Lc79DC.commandName) == "$PQTMCFGGEOFENCE")){
        //poll issued a command, but returned PQTMREQODOERROR, the suspected module firmware does not support
        ui->lineEdit_PQTMCFGGEOFENCE_GeoID->setText(QString::number(msg->Lc79DC.nGeoID));
        ui->comboBox_PQTMCFGGEOFENCE_GeoMode->setCurrentIndex(msg->Lc79DC.nGeoMode);
        ui->comboBox_PQTMCFGGEOFENCE_GeoShape->setCurrentIndex(msg->Lc79DC.nGeoShape);
        ui->lineEdit_PQTMCFGGEOFENCE_Lat0->setText(QString::number(msg->Lc79DC.fLat0,'f',4));
        ui->lineEdit_PQTMCFGGEOFENCE_Lon0->setText(QString::number(msg->Lc79DC.fLon0,'f',4));
        ui->lineEdit_PQTMCFGGEOFENCE_Lat1_Radius->setText(QString::number(msg->Lc79DC.fLat1,'f',4));
        ui->lineEdit_PQTMCFGGEOFENCE_Lon1->setText(msg->Lc79DC.fLon1 > 360? "":QString::number(msg->Lc79DC.fLon1,'f',4));
        ui->lineEdit_PQTMCFGGEOFENCE_Lat2->setText(msg->Lc79DC.fLat2 > 360? "":QString::number(msg->Lc79DC.fLat2,'f',4));
        ui->lineEdit_PQTMCFGGEOFENCE_Lon2->setText(msg->Lc79DC.fLon2 > 360? "":QString::number(msg->Lc79DC.fLon2,'f',4));
        ui->lineEdit_PQTMCFGGEOFENCE_Lat3->setText(msg->Lc79DC.fLat3 > 360? "":QString::number(msg->Lc79DC.fLat3,'f',4));
        ui->lineEdit_PQTMCFGGEOFENCE_Lon3->setText(msg->Lc79DC.fLon3 > 360? "":QString::number(msg->Lc79DC.fLon3,'f',4));

    }else if((tmp_Lc79D_C == "page_PQTMSET_GETGEOFENCE")&&(QString::fromUtf8(msg->Lc79DC.commandName) == "$PQTMGEOFENCE")){
        ui->comboBox_PQTMSET_GETGEOFENCE->setCurrentIndex(msg->Lc79DC.nGeoEn);

    }else if((tmp_Lc79D_C == "page_PQTMREQGEOFENCE")&&(QString::fromUtf8(msg->Lc79DC.commandName) == "$PQTMGEOFENCESTATUS")){
        //poll issued a command, but returned PQTMREQODOERROR, the suspected module firmware does not support
        ui->lineEdit_PQTMREQGEOFENCE->setText(QString::number(msg->Lc79DC.nGeoID));
        ui->comboBox_PQTMREQGEOFENCE->setCurrentIndex(msg->Lc79DC.nGeoStatus);

    }else if((tmp_Lc79D_C == "page_PQTMCFGNMEAMSG")&&(QString::fromUtf8(msg->Lc79DC.commandName) == "$PQTMCFGNMEAMSG")){
        ui->comboBox_PQTMCFGNMEAMSG_NMEA_GGA->setCurrentIndex(msg->Lc79DC.nNMEA_GGA);
        ui->comboBox_PQTMCFGNMEAMSG_NMEA_RMC->setCurrentIndex(msg->Lc79DC.nNMEA_RMC);
        ui->comboBox_PQTMCFGNMEAMSG_NMEA_GSV->setCurrentIndex(msg->Lc79DC.nNMEA_GSV);
        ui->comboBox_PQTMCFGNMEAMSG_NMEA_GSA->setCurrentIndex(msg->Lc79DC.nNMEA_GSA);
        ui->comboBox_PQTMCFGNMEAMSG_NMEA_GLL->setCurrentIndex(msg->Lc79DC.nNMEA_GLL);
        ui->comboBox_PQTMCFGNMEAMSG_NMEA_VTG->setCurrentIndex(msg->Lc79DC.nNMEA_VTG);

    }else if((tmp_Lc79D_C == "page_PQTMCFGEAMASK")&&(QString::fromUtf8(msg->Lc79DC.commandName) == "$PQTMCFGEAMASK")){
        ui->lineEdit_PQTMCFGEAMASK->setText(QString::number(msg->Lc79DC.nEA_Mask));

    }else if((tmp_Lc79D_C == "page_PQTMCFGCLAMPING")&&(QString::fromUtf8(msg->Lc79DC.commandName) == "$PQTMCFGCLAMPING")){
        ui->comboBox_PQTMCFGCLAMPING_Enable->setCurrentIndex(msg->Lc79DC.nEnable);
    }
}

void configuration_LC79DC::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    ui->stackedWidget->setCurrentWidget(ui->stackedWidget->findChildren<QWidget *>(current->text(1))[0]);
}

void configuration_LC79DC::on_pushButton_send_clicked()
{
    QByteArray text = (ui->lineEditCommand->text()+"\r\n").toUtf8();
    emit sendData(text.data(), text.size());
    qDebug()<<" QByteArray_LC79DC:"<<text;
}

void configuration_LC79DC::on_GC_PQTMCOLD_clicked()
{
    QByteArray cmd("$PQTMCOLD");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_GC_PQTMWARM_clicked()
{
    QByteArray cmd("$PQTMWARM");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_GC_PQTMHOT_clicked()
{
    QByteArray cmd("$PQTMHOT");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_GC_PQTMSRR_clicked()
{
    QByteArray cmd("$PQTMSRR");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_GC_PQTMSET_GETGLP_clicked()
{
    QByteArray cmd("$PQTMSETGLP,");
    cmd.append(QString::number(ui->comboBox_PQTMSET_GETGLP->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_poll_PQTMSET_GETGLP_clicked()
{
    QByteArray text("$PQTMGETGLP");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DC::on_GC_PQTMSETBAUD_clicked()
{
    QByteArray cmd("$PQTMSETBAUD,");
    switch (ui->comboBox_PQTMSETBAUD->currentIndex()) {
    case 0:
        cmd.append("115200");
        break;
    case 1:
        cmd.append("230400");
        break;
    case 2:
        cmd.append("460800");
        break;
    case 3:
        cmd.append("921600");
        break;
    default:
        break;
    }
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_GC_PQTMSET_GETCNST_clicked()
{
    int tmp_Hex = 0;
    tmp_Hex |= ui->checkBox_PQTMSET_GETCNST_GPS->isChecked()?     0x1 :0x0;
    tmp_Hex |= ui->checkBox_PQTMSET_GETCNST_GLONASS->isChecked()? 0x2 :0x0;
    tmp_Hex |= ui->checkBox_PQTMSET_GETCNST_Galileo->isChecked()? 0x4 :0x0;
    tmp_Hex |= ui->checkBox_PQTMSET_GETCNST_BDS->isChecked()?     0x8 :0x0;
    tmp_Hex |= ui->checkBox_PQTMSET_GETCNST_NAVIC->isChecked()?   0x10:0x0;
    tmp_Hex |= ui->checkBox_PQTMSET_GETCNST_QZSS->isChecked()?    0x20:0x0;
    QByteArray cmd("$PQTMSETCNST,");
    cmd.append(QString::number(tmp_Hex));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_poll_PQTMSET_GETCNST_clicked()
{
    QByteArray text("$PQTMGETCNST");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DC::on_GC_PQTMSET_GETL5BIAS_clicked()
{
    QByteArray cmd("$PQTMSETL5BIAS,");
    cmd.append(ui->lineEdit_PQTMSET_GETL5BIAS->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_poll_PQTMSET_GETL5BIAS_clicked()
{
    QByteArray text("$PQTMGETL5BIAS");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DC::on_GC_PQTMCFGODO_clicked()
{
    QByteArray cmd("$PQTMCFGODO,1,");
    cmd.append(QString::number(ui->comboBox_PQTMCFGODO_Auto_Start->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGODO_MSG_Mode->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGODO_InitVal->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_poll_PQTMCFGODO_clicked()
{
    QByteArray text("$PQTMCFGODO,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DC::on_poll_PQTMREQODO_clicked()
{
    QByteArray text("$PQTMREQODO");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DC::on_GC_PQTMRESETODO_clicked()
{
    QByteArray cmd("$PQTMRESETODO");
    cmd.append(ui->lineEdit_PQTMSET_GETL5BIAS->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_GC_PQTMSTARTODO_clicked()
{
    QByteArray cmd("$PQTMSTARTODO");
    cmd.append(ui->lineEdit_PQTMSET_GETL5BIAS->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_GC_PQTMSTOPODO_clicked()
{
    QByteArray cmd("$PQTMSTOPODO");
    cmd.append(ui->lineEdit_PQTMSET_GETL5BIAS->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_GC_PQTMCFGGEOFENCE_clicked()
{
    QByteArray cmd("$PQTMCFGGEOFENCE,1,");
    cmd.append(ui->lineEdit_PQTMCFGGEOFENCE_GeoID->text());
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGGEOFENCE_GeoMode->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGGEOFENCE_GeoShape->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGGEOFENCE_Lat0->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGGEOFENCE_Lon0->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMCFGGEOFENCE_Lat1_Radius->text());
    cmd.append(ui->lineEdit_PQTMCFGGEOFENCE_Lon1->text().isEmpty()? "": ("," + ui->lineEdit_PQTMCFGGEOFENCE_Lon1->text()));
    cmd.append(ui->lineEdit_PQTMCFGGEOFENCE_Lat2->text().isEmpty()? "": ("," + ui->lineEdit_PQTMCFGGEOFENCE_Lat2->text()));
    cmd.append(ui->lineEdit_PQTMCFGGEOFENCE_Lon2->text().isEmpty()? "": ("," + ui->lineEdit_PQTMCFGGEOFENCE_Lon2->text()));
    cmd.append(ui->lineEdit_PQTMCFGGEOFENCE_Lat3->text().isEmpty()? "": ("," + ui->lineEdit_PQTMCFGGEOFENCE_Lat3->text()));
    cmd.append(ui->lineEdit_PQTMCFGGEOFENCE_Lon3->text().isEmpty()? "": ("," + ui->lineEdit_PQTMCFGGEOFENCE_Lon3->text()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_poll_PQTMCFGGEOFENCE_clicked()
{
    QByteArray text("$PQTMCFGGEOFENCE,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DC::on_GC_PQTMSET_GETGEOFENCE_clicked()
{
    QByteArray cmd("$PQTMSETGEOFENCE,");
    cmd.append(QString::number(ui->comboBox_PQTMSET_GETGEOFENCE->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_poll_PQTMSET_GETGEOFENCE_clicked()
{
    QByteArray text("$PQTMGETGEOFENCE");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}
//Requires user to enter GeoID to query
void configuration_LC79DC::on_poll_PQTMREQGEOFENCE_clicked()
{
    QByteArray text("$PQTMREQGEOFENCE,");
    text.append(ui->lineEdit_PQTMREQGEOFENCE->text());
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DC::on_GC_PQTMCFGNMEAMSG_clicked()
{
    QByteArray cmd("$PQTMCFGNMEAMSG,1,");
    cmd.append(QString::number(ui->comboBox_PQTMCFGNMEAMSG_NMEA_GGA->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGNMEAMSG_NMEA_RMC->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGNMEAMSG_NMEA_GSV->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGNMEAMSG_NMEA_GSA->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGNMEAMSG_NMEA_GLL->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_PQTMCFGNMEAMSG_NMEA_VTG->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_poll_PQTMCFGNMEAMSG_clicked()
{
    QByteArray text("$PQTMCFGNMEAMSG,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DC::on_GC_PQTMCFGEAMASK_clicked()
{
    QByteArray cmd("$PQTMCFGEAMASK,1,");
    cmd.append(ui->lineEdit_PQTMCFGEAMASK->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_poll_PQTMCFGEAMASK_clicked()
{
    QByteArray text("$PQTMCFGEAMASK,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DC::on_GC_PQTMCFGCLAMPING_clicked()
{
    QByteArray cmd("$PQTMCFGCLAMPING,1,");
    cmd.append(QString::number(ui->comboBox_PQTMCFGCLAMPING_Enable->currentIndex()));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_poll_PQTMCFGCLAMPING_clicked()
{
    QByteArray text("$PQTMCFGCLAMPING,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LC79DC::on_GC_PQTMSAVEPAR_clicked()
{
    QByteArray cmd("$PQTMSAVEPAR");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_GC_PQTMRESTOREPAR_clicked()
{
    QByteArray cmd("$PQTMRESTOREPAR");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LC79DC::on_GC_PQTMSETINSMSG_clicked()
{
    QByteArray cmd("$PQTMSETINSMSG,");
    cmd.append(QString::number(ui->comboBox_imu_onOff->currentIndex()));
    cmd.append(",");
    cmd.append(QString::number(ui->comboBox_ins_onOff->currentIndex()));
    cmd.append(",");
    int rate = ui->lineEdit_imu_ins_rate->text().toInt();
    if(rate>=0 && rate<=70){

    }
    else {
        rate = 0;
    }
    cmd.append(QString::number(rate));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

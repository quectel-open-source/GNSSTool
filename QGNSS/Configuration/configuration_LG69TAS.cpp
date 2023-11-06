#include "configuration_LG69TAS.h"
#include "ui_configuration_LG69TAS.h"
#include <QtDebug>

configuration_LG69TAS::configuration_LG69TAS(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::configuration_LG69TAS)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->treeWidget->setColumnWidth(0,400);
    this->setAttribute(Qt::WA_QuitOnClose,false);
    this->setWindowTitle("Configuration:LG69TAS");
    ui->treeWidget->hideColumn(1);
    on_comboBox_PQTMRECVMODE_currentIndexChanged(1);
}

configuration_LG69TAS::~configuration_LG69TAS()
{
    delete ui;
}

void configuration_LG69TAS::updateUI(NMEA_t *msg)
{
    QString tmp_LG69T_AS = ui->stackedWidget->currentWidget()->objectName();

    if((tmp_LG69T_AS == "page_PQTMCFGPORT")&&(QString::fromUtf8(msg->LG69TAS.commandName) == "$PQTMPORT")){
        ui->comboBox_PortType->setCurrentIndex(msg->LG69TAS.nPortType);
        (msg->LG69TAS.nProtocolType&0x1)!=0 ? ui->checkBox_NMEAInput->setCheckState(Qt::Checked):ui->checkBox_NMEAInput->setCheckState(Qt::Unchecked);
        ((msg->LG69TAS.nProtocolType>>1)&0x1)!=0 ? ui->checkBox_NMEAOutput->setCheckState(Qt::Checked):ui->checkBox_NMEAOutput->setCheckState(Qt::Unchecked);
        ((msg->LG69TAS.nProtocolType>>2)&0x1)!=0 ? ui->checkBox_RTCMOutput->setCheckState(Qt::Checked):ui->checkBox_RTCMOutput->setCheckState(Qt::Unchecked);
        switch (msg->LG69TAS.nBaudRate) {
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

    }else if((tmp_LG69T_AS == "page_PQTM_SET_GET_RECVMODE")&&(QString::fromUtf8(msg->LG69TAS.commandName) == "$PQTMRECVMODE")){
        ui->comboBox_PQTMRECVMODE->setCurrentIndex(msg->LG69TAS.nMode);
        ui->lineEdit_PQTMRECVMODE_Du->setText(QString::number(msg->LG69TAS.nDuration));
        ui->lineEdit_PQTMRECVMODE_AH->setText(QString::number(msg->LG69TAS.dAntHeight,'f',4));
        ui->lineEdit_PQTMRECVMODE_X->setText(QString::number(msg->LG69TAS.dECEF_X,'f',4));
        ui->lineEdit_PQTMRECVMODE_Y->setText(QString::number(msg->LG69TAS.dECEF_Y,'f',4));
        ui->lineEdit_PQTMRECVMODE_Z->setText(QString::number(msg->LG69TAS.dECEF_Z,'f',4));

    }else if((tmp_LG69T_AS == "page_PQTM_SET_GET_MSMMODE")&&(QString::fromUtf8(msg->LG69TAS.commandName) == "$PQTMMSMMODE")){
        msg->LG69TAS.nMode ==4 ? ui->comboBox_PQTMMSMMODE->setCurrentIndex(0):ui->comboBox_PQTMMSMMODE->setCurrentIndex(1);

    }else if((tmp_LG69T_AS == "page_PQTM_SET_GET_CONSTMASK")&&(QString::fromUtf8(msg->LG69TAS.commandName) == "$PQTMCONSTMASK")){
        (msg->LG69TAS.nMask&0x1)     !=0 ? ui->checkBox_GPS->setCheckState(Qt::Checked):ui->checkBox_GPS->setCheckState(Qt::Unchecked);
        ((msg->LG69TAS.nMask>>2)&0x1)!=0 ? ui->checkBox_Galileo->setCheckState(Qt::Checked):ui->checkBox_Galileo->setCheckState(Qt::Unchecked);
        ((msg->LG69TAS.nMask>>3)&0x1)!=0 ? ui->checkBox_BDS->setCheckState(Qt::Checked):ui->checkBox_BDS->setCheckState(Qt::Unchecked);
        ((msg->LG69TAS.nMask>>4)&0x1)!=0 ? ui->checkBox_QZSS->setCheckState(Qt::Checked):ui->checkBox_QZSS->setCheckState(Qt::Unchecked);

    }else if((tmp_LG69T_AS == "page_PQTMVERNO")&&(QString::fromUtf8(msg->LG69TAS.commandName) == "$PQTMVERNO")){
        ui->lineEdit_MainVersion->setText(QString::fromUtf8(msg->LG69TAS.mainVersion));
    }
}

void configuration_LG69TAS::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    ui->stackedWidget->setCurrentWidget(ui->stackedWidget->findChildren<QWidget *>(current->text(1))[0]);
}

void configuration_LG69TAS::on_pushButton_send_clicked()
{
    QByteArray text = (ui->lineEditCommand->text()+"\r\n").toUtf8();
    emit sendData(text.data(), text.size());
    qDebug()<<" QByteArray_LG69TAS:"<<text;
}

void configuration_LG69TAS::on_GC_PQTMSRR_clicked()
{
    QByteArray cmd("$PQTMSRR");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAS::on_GC_PQTMCFGPORT_clicked()
{
    int tmpBit=0;QString tmp;
    QByteArray cmd("$PQTMCFGPORT,1,");
    cmd.append(QString::number(ui->comboBox_PortType->currentIndex()));
    cmd.append(",");
    tmpBit |= ui->checkBox_NMEAInput->isChecked()?  0x1:0x0;
    tmpBit |= ui->checkBox_NMEAOutput->isChecked()? 0x2:0x0;
    tmpBit |= ui->checkBox_RTCMOutput->isChecked()? 0x4:0x0;
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

void configuration_LG69TAS::on_poll_PQTMCFGPORT_clicked()
{
    QByteArray text("$PQTMCFGPORT,0,0");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LG69TAS::on_GC_PQTM_SET_GET_RECVMODE_clicked()
{
    QByteArray cmd("$PQTMSETRECVMODE,");
    cmd.append(QString::number(ui->comboBox_PQTMRECVMODE->currentIndex()));
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMRECVMODE_Du->isHidden()?"0":ui->lineEdit_PQTMRECVMODE_Du->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMRECVMODE_AH->isHidden()?"0":ui->lineEdit_PQTMRECVMODE_AH->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMRECVMODE_X->isHidden()?"0":ui->lineEdit_PQTMRECVMODE_X->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMRECVMODE_Y->isHidden()?"0":ui->lineEdit_PQTMRECVMODE_Y->text());
    cmd.append(",");
    cmd.append(ui->lineEdit_PQTMRECVMODE_Z->isHidden()?"0":ui->lineEdit_PQTMRECVMODE_Z->text());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAS::on_poll_PQTM_SET_GET_RECVMODE_clicked()
{
    QByteArray text("$PQTMGETRECVMODE");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LG69TAS::on_GC_PQTM_SET_GET_MSMMODE_clicked()
{
    QByteArray cmd("$PQTMSETMSMMODE,");
    ui->comboBox_PQTMMSMMODE->currentIndex()!=0?cmd.append("7"):cmd.append("4");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAS::on_poll_PQTM_SET_GET_MSMMODE_clicked()
{
    QByteArray text("$PQTMGETMSMMODE");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LG69TAS::on_GC_PQTM_SET_GET_CONSTMASK_clicked()
{
    int tmpBit=0;
    QByteArray cmd("$PQTMSETCONSTMASK,");
    tmpBit |= ui->checkBox_GPS->isChecked()?     0x1 : 0x0;
    tmpBit |= ui->checkBox_Galileo->isChecked()? 0x4 : 0x0;
    tmpBit |= ui->checkBox_BDS->isChecked()?     0x8 : 0x0;
    tmpBit |= ui->checkBox_QZSS->isChecked()?    0x10: 0x0;
    cmd.append(QString::number(tmpBit));
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAS::on_poll_PQTM_SET_GET_CONSTMASK_clicked()
{
    QByteArray text("$PQTMGETCONSTMASK");
    addNMEA_CS(text);
    emit sendData(text.data(), text.size());
}

void configuration_LG69TAS::on_GC_PQTMSAVEPAR_clicked()
{
    QByteArray cmd("$PQTMSAVEPAR");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAS::on_GC_PQTMRESTOREPAR_clicked()
{
    QByteArray cmd("$PQTMRESTOREPAR");
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void configuration_LG69TAS::on_poll_PQTMVERNO_clicked()
{
    //Query the main version string
    QByteArray text("$PQTMVERNO*58\r\n");
    emit sendData(text.data(), text.size());
}

void configuration_LG69TAS::on_comboBox_PQTMRECVMODE_currentIndexChanged(int index)
{
    if(index == 0){
        ui->label_Du->setHidden(true );
        ui->label_AH->setHidden(false);
        ui->label_X ->setHidden(true );
        ui->label_Y ->setHidden(true );
        ui->label_Z ->setHidden(true );
        ui->lineEdit_PQTMRECVMODE_Du->setHidden(true );
        ui->lineEdit_PQTMRECVMODE_AH->setHidden(false);
        ui->lineEdit_PQTMRECVMODE_X ->setHidden(true );
        ui->lineEdit_PQTMRECVMODE_Y ->setHidden(true );
        ui->lineEdit_PQTMRECVMODE_Z ->setHidden(true );

    }else if(index == 1){
        ui->label_Du->setHidden(false);
        ui->label_AH->setHidden(false);
        ui->label_X ->setHidden(true );
        ui->label_Y ->setHidden(true );
        ui->label_Z ->setHidden(true );
        ui->lineEdit_PQTMRECVMODE_Du->setHidden(false);
        ui->lineEdit_PQTMRECVMODE_AH->setHidden(false);
        ui->lineEdit_PQTMRECVMODE_X ->setHidden(true );
        ui->lineEdit_PQTMRECVMODE_Y ->setHidden(true );
        ui->lineEdit_PQTMRECVMODE_Z ->setHidden(true );
    }else if(index == 2){
        ui->label_Du->setHidden(false);
        ui->label_AH->setHidden(false);
        ui->label_X ->setHidden(false);
        ui->label_Y ->setHidden(false);
        ui->label_Z ->setHidden(false);
        ui->lineEdit_PQTMRECVMODE_Du->setHidden(false);
        ui->lineEdit_PQTMRECVMODE_AH->setHidden(false);
        ui->lineEdit_PQTMRECVMODE_X ->setHidden(false);
        ui->lineEdit_PQTMRECVMODE_Y ->setHidden(false);
        ui->lineEdit_PQTMRECVMODE_Z ->setHidden(false);
    }
    ui->lineEdit_PQTMRECVMODE_Du->clear();
    ui->lineEdit_PQTMRECVMODE_AH->clear();
    ui->lineEdit_PQTMRECVMODE_X ->clear();
    ui->lineEdit_PQTMRECVMODE_Y ->clear();
    ui->lineEdit_PQTMRECVMODE_Z ->clear();
}

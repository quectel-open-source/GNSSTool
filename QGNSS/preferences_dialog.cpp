#include "preferences_dialog.h"
#include "ui_preferences_dialog.h"
#include "common.h"

#include <QPushButton>
#include <QDebug>

Preferences_Dialog::Preferences_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Preferences_Dialog)
{
    ui->setupUi(this);
    //隐藏供应商信息
    ui->checkBox_P1->setHidden(true);
    ui->checkBox_MTK->setHidden(true);
    ui->checkBox_CASIC->setHidden(true);
    ui->checkBox_Broadcom->setHidden(true);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowTitle("Preferences");
    setWindowFlags(Qt::Dialog | Qt::WindowCloseButtonHint);
    settings = new QSettings(SETTINGS_FILE_NAME, QSettings::IniFormat);
    on_listWidget_currentRowChanged(999);
}

Preferences_Dialog::~Preferences_Dialog()
{
    delete ui;
    delete settings;
}

void Preferences_Dialog::on_buttonBox_clicked(QAbstractButton *button)
{
    QDialogButtonBox::ButtonRole bRole =  ui->buttonBox->buttonRole(button);
    switch (bRole) {
    case QDialogButtonBox::ApplyRole:
    case QDialogButtonBox::AcceptRole:
        //TianDiMap key has been changed
        if(settings->value(TianDiMapKey).toString() != ui->lineEdit_mapKey->text())
        {
            settings->setValue(TianDiMapKey,QVariant(ui->lineEdit_mapKey->text()));
            emit applyMapSettings();
        }
        //Parser settings
        settings->setValue(parseMTK_Key, ui->checkBox_MTK->isChecked());
        settings->setValue(parseNMEA_Key, ui->checkBox_NMEA->isChecked());
        settings->setValue(parseOther, ui->checkBox_Other->isChecked());
        settings->setValue(parseRTCM_Key, ui->checkBox_RTCM->isChecked());
        settings->setValue(parseCASIC_Key, ui->checkBox_CASIC->isChecked());
        settings->setValue(parseBroad_Key, ui->checkBox_Broadcom->isChecked());
        settings->setValue(parseP1_Key, ui->checkBox_P1->isChecked());
        //module log
        settings->setValue(LogType_Key, ui->groupBox_Size->isChecked());
        settings->setValue(MAX_Log_Key, ui->comboBox_LogSize->currentText());
        settings->setValue(DailyTime_Key, ui->timeEdit_Daily->dateTime());
        emit applySettings();
        if(bRole == QDialogButtonBox::AcceptRole)
            this->close();
        break;
    case QDialogButtonBox::RejectRole:
        qDebug()<< "rejected";
        this->close();
        break;
    default:
        qDebug()<< "unknow button";
        break;
    }

}

void Preferences_Dialog::on_checkBox_Other_stateChanged(int arg1)
{
    ui->checkBox_P1->setChecked(arg1);
    ui->checkBox_MTK->setChecked(arg1);
    ui->checkBox_CASIC->setChecked(arg1);
    ui->checkBox_Broadcom->setChecked(arg1);
}

void Preferences_Dialog::on_groupBox_Size_clicked(bool checked)
{
    qDebug()<<"on_groupBox_Size_clicked"<<checked;
    ui->groupBox_Daily->setChecked(!checked);
}

void Preferences_Dialog::on_groupBox_Daily_clicked(bool checked)
{
    qDebug()<<"on_groupBox_Daily_clicked"<<checked;
    ui->groupBox_Size->setChecked(!checked);
}

void Preferences_Dialog::on_listWidget_currentRowChanged(int currentRow)
{
    qDebug()<<"on_listWidget_currentRowChanged"<<currentRow;
    ui->lineEdit_mapKey->setText(settings->value(TianDiMapKey).toString());

    //parser
    ui->checkBox_MTK->setChecked(settings->value(parseOther, true).toBool());
    ui->checkBox_Other->setChecked(settings->value(parseMTK_Key, true).toBool());
    ui->checkBox_CASIC->setChecked(settings->value(parseCASIC_Key, true).toBool());
    ui->checkBox_RTCM->setChecked(settings->value(parseRTCM_Key, true).toBool());
    ui->checkBox_NMEA->setChecked(settings->value(parseNMEA_Key, true).toBool());
    ui->checkBox_Broadcom->setChecked(settings->value(parseBroad_Key, true).toBool());
    ui->checkBox_P1->setChecked(settings->value(parseP1_Key, true).toBool());

    //module log
    bool type = settings->value(LogType_Key, true).toBool();
    ui->groupBox_Size->setChecked(type);
    ui->groupBox_Daily->setChecked(!type);
    QString v = settings->value(MAX_Log_Key, "2 GB").toString();
    for(int i = 0; i<ui->comboBox_LogSize->count();i++){
        if(ui->comboBox_LogSize->itemData(i, Qt::DisplayRole).toString() == v)
            ui->comboBox_LogSize->setCurrentIndex(i);
    }
    auto t = QDateTime::fromString("01:00", "hh:mm");
    ui->timeEdit_Daily->setDateTime(settings->value(DailyTime_Key, t).toDateTime());

}

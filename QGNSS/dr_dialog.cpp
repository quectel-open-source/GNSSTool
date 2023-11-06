#include "dr_dialog.h"
#include "ui_dr_dialog.h"

DR_Dialog::DR_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DR_Dialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setAttribute(Qt::WA_DeleteOnClose, true);
    fTimer = new QTimer(this);
    fTimer->setInterval(50);
    connect(fTimer,SIGNAL(timeout()),this,SLOT(on_timer_timeout()));
    fTimer->start();
}

DR_Dialog::~DR_Dialog()
{
    delete ui;
    delete fTimer;
}

void DR_Dialog::setNMEA_Data_t(NMEA_t *value)
{
    NMEA_Data_t = value;
}

void DR_Dialog::on_timer_timeout()
{
    QString styleEnable = "color: black;background-color: green";
    if(NMEA_Data_t){
        ui->pushButton_drFull->setStyleSheet(NMEA_Data_t->DR_Data.dr_is_calib?styleEnable:"");
        ui->pushButton_odoScale->setStyleSheet(NMEA_Data_t->DR_Data.odo_is_calib?styleEnable:"");
        ui->pushButton_gyroGain->setStyleSheet(NMEA_Data_t->DR_Data.gyro_sensitivity_is_calib?styleEnable:"");
        ui->pushButton_gyroOffset->setStyleSheet(NMEA_Data_t->DR_Data.gyro_bias_is_calib?styleEnable:"");

        if(NMEA_Data_t->DR_Data.NavModeInfo==60){
            ui->lineEdit_drMode->setText("Dead Reckoning");
        }else if(NMEA_Data_t->DR_Data.NavModeInfo&0x02){
            ui->lineEdit_drMode->setText(NMEA_Data_t->DR_Data.NavModeInfo&0x02?"GNSS only":"");
        }else if(NMEA_Data_t->DR_Data.NavModeInfo&0x04){
            ui->lineEdit_drMode->setText(NMEA_Data_t->DR_Data.NavModeInfo&0x04?"Sensor only":"");
        }else if(NMEA_Data_t->DR_Data.NavModeInfo&0x08){
            ui->lineEdit_drMode->setText(NMEA_Data_t->DR_Data.NavModeInfo&0x08?"Combined solution":"");
        }else{
            ui->lineEdit_drMode->setText("");
        }
    }
}

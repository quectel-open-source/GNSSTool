#include "inputactuallocationdialog.h"
#include "ui_inputactuallocationdialog.h"
#include "gps_nmea.h"
#include "QMessageBox"
#include "QSettings"

InputActualLocationDialog::InputActualLocationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputActualLocationDialog)
{
    ui->setupUi(this);
    loadSettingData();
}

InputActualLocationDialog::~InputActualLocationDialog()
{
    delete ui;
}

void InputActualLocationDialog::on_buttonBox_accepted()
{
    //TODO: check the input
    if (ui->inputLatitude_lineEdit->text().isEmpty() || ui->inputHeight_LineEdit->text().isEmpty() || ui->inputLongitude_lineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Warning", "Input actual location is NULL!");
        emit reject();
    }
    else
    {
        lla.lon = ui->inputLongitude_lineEdit->text().toDouble();
        lla.lat = ui->inputLatitude_lineEdit->text().toDouble();
        lla.alt = ui->inputHeight_LineEdit->text().toDouble();
        loadSettingData(true);
    }
}

void InputActualLocationDialog::loadSettingData(bool rw)
{
    QSettings settings;
    if(rw){
        settings.setValue("ActualLocation/Longitude", ui->inputLongitude_lineEdit->text());
        settings.setValue("ActualLocation/Latitude", ui->inputLatitude_lineEdit->text());
        settings.setValue("ActualLocation/Height", ui->inputHeight_LineEdit->text());
        return ;
    }
    ui->inputLongitude_lineEdit->setText(settings.value("ActualLocation/Longitude", "117.11519539").toString());
    ui->inputLatitude_lineEdit->setText(settings.value("ActualLocation/Latitude", "31.82203967").toString());
    ui->inputHeight_LineEdit->setText(settings.value("ActualLocation/Height", "51.162").toString());
}

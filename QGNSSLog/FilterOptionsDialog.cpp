#include "FilterOptionsDialog.h"
#include "ui_FilterOptionsDialog.h"
#include <QSettings>
#include <QDebug>

FilterOptionsDialog::FilterOptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FilterOptionsDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("Filter Options");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    filterOptionsDialogRead();
}

FilterOptionsDialog::~FilterOptionsDialog()
{
    delete ui;
}

void FilterOptionsDialog::filterOptionsDialogRead()
{
    QLOG_CFG *logcfg = new QLOG_CFG;
    QSettings setting("./config/FilterOptions.ini", QSettings::IniFormat);
    logcfg->Sinfo.SatElev = (char)setting.value("Elevation").toInt();
    logcfg->Sinfo.SatAz   = setting.value("Azimuth").toInt();
    logcfg->Sinfo.SatSNR  = (char)setting.value("SNRCNo").toInt();
    Set_Qlog_cfg(logcfg);
    ui->spinBox_Elevation->setValue(setting.value("Elevation").toInt());
    ui->spinBox_Azimuth->setValue(setting.value("Azimuth").toInt());
    ui->spinBox_SNRCNo->setValue(setting.value("SNRCNo").toInt());
}

void FilterOptionsDialog::filterOptionsDialogWrite()
{
    QString Elevation= ui->spinBox_Elevation->text();
    QString Azimuth  = ui->spinBox_Azimuth->text();
    QString SNRCNo   = ui->spinBox_SNRCNo->text();
    QSettings setting("./config/FilterOptions.ini", QSettings::IniFormat);
    setting.setValue("Elevation", Elevation);
    setting.setValue("Azimuth"  , Azimuth  );
    setting.setValue("SNRCNo"   , SNRCNo   );
}

void FilterOptionsDialog::on_pushButton_OK_clicked()
{
    filterOptionsDialogWrite();
    filterOptionsDialogRead();
    this->close();
}

void FilterOptionsDialog::on_pushButton_Cancel_clicked()
{
    this->close();
}

void FilterOptionsDialog::on_pushButton_Apply_clicked()
{
    filterOptionsDialogWrite();
    filterOptionsDialogRead();
}

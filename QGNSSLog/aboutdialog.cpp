#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "logplot_mainwindow.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    this->setWindowTitle("About QGNSSLog");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    QString tlog = "QGNSS ";
    ui->label_Main->setText(tlog+LOG_VERSION);
    ui->label_Bulit->setText("Built on Nov 28 2022 15:22:00 ");
    ui->label_contactUsURL->setOpenExternalLinks(true);
    ui->label_contactUsURL->setText(tr("<a href=\"https://www.quectel.com/support/contact.htm\">www.quectel.com/support/contact.htm"));
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::on_pushButton_clicked()
{
    this->close();
}

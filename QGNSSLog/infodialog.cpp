#include "infodialog.h"
#include "ui_infodialog.h"

infoDialog::infoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::infoDialog)
{
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    ui->setupUi(this);  
    this->setWindowTitle("Help Manual");

    QPixmap pixmap;
    pixmap.load(":/images/MenuBar.png");
    ui->labelImage_1->setPixmap(pixmap);

    pixmap.load(":/images/ZOOM.jpg");
    ui->labelImage_2->setPixmap(pixmap);

    pixmap.load(":/images/Plotting Area.jpg");
    ui->labelImage_3->setPixmap(pixmap);

    pixmap.load(":/images/Plots.png");
    ui->labelImage_4->setPixmap(pixmap);

    pixmap.load(":/images/Signals.jpg");
    ui->labelImage_5->setPixmap(pixmap);
}

infoDialog::~infoDialog()
{
    delete ui;
}

void infoDialog::on_listWidget_currentRowChanged(int currentRow)
{
    ui->stackedWidget->setCurrentIndex(currentRow);
}

#include "about.h"
#include "ui_about.h"

#include <QDebug>
#include <QKeyEvent>


const QDateTime buildDateTime()
{
    QString dateTime;
    foreach (auto var, QString(__DATE__).split(' ',QString::SkipEmptyParts)) {
            dateTime+=var;
            dateTime+=" ";
    }
    dateTime += __TIME__;
    return QLocale(QLocale::English).toDateTime(dateTime,"MMM d yyyy hh:mm:ss");
}

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setAttribute(Qt::WA_QuitOnClose,false);
    this->setAttribute(Qt::WA_DeleteOnClose,true);
    QPalette palette(this->palette());
    palette.setColor(QPalette::Background, Qt::white);
    this->setPalette(palette);
    this->setWindowTitle("About");

    ui->label_version->setText(QString("Version: %1").arg(App_mainVersion));
    ui->label_build->setText(QString("Build: %1").arg(buildDateTime().toString("yyyy-MM-dd")));

//    ui->label_contactUsURL->setOpenExternalLinks(true);
//    ui->label_contactUsURL->setText(tr("<a href=\"https://www.quectel.com/support/contact.htm\">www.quectel.com/support/contact.htm"));
}

About::~About()
{
    delete ui;
}

void About::keyPressEvent(QKeyEvent *ev)
{

    if(ev->modifiers() == (Qt::ShiftModifier|Qt::AltModifier))
    {
        if(ev->key() == Qt::Key_Q)
        {
            // show internal version
            ui->label_version->setText(QString("Version: %1.%2").arg(App_mainVersion).arg(App_subVersion));
            ui->label_build->setText(QString("Build: %1").arg(buildDateTime().toString("yyyy-MM-dd hh:mm:ss")));
        }
    }
}

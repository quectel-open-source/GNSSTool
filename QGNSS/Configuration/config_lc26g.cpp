#include "config_lc26g.h"
#include "ui_config_lc26g.h"

#include <QDebug>
#include <QMetaEnum>

Config_LC26G::Config_LC26G(QWidget *parent, const quectel_gnss::QGNSS_Model &value) :
    QDialog(parent),
    ui(new Ui::Config_LC26G)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    this->setAttribute(Qt::WA_DeleteOnClose);
    QMetaEnum curType = QMetaEnum::fromType<quectel_gnss::QGNSS_Model>();
    this->setWindowTitle(QString("Configuration:")+curType.valueToKey(value));
}

Config_LC26G::~Config_LC26G()
{
    qDebug()<<"~Config_LC26G";
    delete ui;
}

void Config_LC26G::on_pushButton_GC_baudrate_clicked()
{
    QByteArray cmd("$PAIR864,0,0,");
    cmd.append(ui->comboBox_baudrate->currentText());
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

void Config_LC26G::on_pushButton_send_clicked()
{    
    QByteArray text = (ui->lineEditCommand->text()+"\r\n").toLocal8Bit();
    emit sendCMD(text);
}

void Config_LC26G::on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous)
{
    ui->stackedWidget->setCurrentWidget(ui->stackedWidget->findChildren<QWidget *>("page_"+current->text(0))[0]);
}

void Config_LC26G::on_pushButton_GC_SEARCH_clicked()
{
    QString cmd = QString("$PAIR066,%1,%2,%3,%4,%5,0")
            .arg(ui->checkBox_gps->isChecked()?1:0)
            .arg(ui->checkBox_glo->isChecked()?1:0)
            .arg(ui->checkBox_gal->isChecked()?1:0)
            .arg(ui->checkBox_bds->isChecked()?1:0)
            .arg(ui->checkBox_qzss->isChecked()?1:0)
            ;
    addNMEA_MC_CS(cmd);
    ui->lineEditCommand->setText(cmd);
}

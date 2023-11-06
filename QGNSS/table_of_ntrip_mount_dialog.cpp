#include "table_of_ntrip_mount_dialog.h"
#include "ui_table_of_ntrip_mount_dialog.h"

Table_Of_NTRIP_Mount_Dialog::Table_Of_NTRIP_Mount_Dialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Table_Of_NTRIP_Mount_Dialog)
{
    ui->setupUi(this);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    model = new QStandardItemModel(this);
    QStringList headStr;
    headStr<<"Name"<<"Source identifier"<<"Formats supported"<<"Formats details"<<
             "Carrier"<<"Navigation system"<<"Network"<<"Country"<<"Latitude"<<
             "Longitude"<<"NMEA"<<"Solution"<<"Comperssion"<<"Fee"<<"Bit rate";
    model->setColumnCount(headStr.count());
    int i = 0;
    foreach (auto str, headStr) {
        model->setHeaderData(i,Qt::Horizontal, str);
        i++;
    }

     ui->tableView->setModel(model);
     ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

Table_Of_NTRIP_Mount_Dialog::~Table_Of_NTRIP_Mount_Dialog()
{
    delete ui;
}

void Table_Of_NTRIP_Mount_Dialog::insertMountPointInfo2Table(QStringList &strList)
{

    QList<QStandardItem*> list;
    foreach (auto str, strList) {
        list.append(new QStandardItem(str));
    }
    model->appendRow(list);
}

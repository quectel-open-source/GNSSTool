#ifndef CONFIGURATION_LG69TAS_H
#define CONFIGURATION_LG69TAS_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "QQueue"
#include "QByteArray"
#include "QStatusBar"
#include "QDateTime"
#include "common.h"
#include "DataParser/nmea_parser.h"


namespace Ui {
class configuration_LG69TAS;
}

class configuration_LG69TAS : public QDialog
{
    Q_OBJECT

public:
    explicit configuration_LG69TAS(QWidget *parent = nullptr);
    ~configuration_LG69TAS();
signals:
    void  sendData(const char * data, unsigned int size);
public  slots:
       void updateUI(NMEA_t *msg);
private slots:
       void on_GC_PQTMSRR_clicked();

       void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

       void on_pushButton_send_clicked();

       void on_GC_PQTMCFGPORT_clicked();

       void on_poll_PQTMCFGPORT_clicked();

       void on_GC_PQTM_SET_GET_RECVMODE_clicked();

       void on_poll_PQTM_SET_GET_RECVMODE_clicked();

       void on_GC_PQTM_SET_GET_MSMMODE_clicked();

       void on_poll_PQTM_SET_GET_MSMMODE_clicked();

       void on_GC_PQTM_SET_GET_CONSTMASK_clicked();

       void on_poll_PQTM_SET_GET_CONSTMASK_clicked();

       void on_GC_PQTMSAVEPAR_clicked();

       void on_GC_PQTMRESTOREPAR_clicked();

       void on_poll_PQTMVERNO_clicked();

       void on_comboBox_PQTMRECVMODE_currentIndexChanged(int index);
private:
    Ui::configuration_LG69TAS *ui;
};

#endif // CONFIGURATION_LG69TAS_H

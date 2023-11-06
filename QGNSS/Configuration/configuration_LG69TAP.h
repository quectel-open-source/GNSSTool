#ifndef CONFIGURATION_LG69TAP_H
#define CONFIGURATION_LG69TAP_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "QQueue"
#include "QByteArray"
#include "QStatusBar"
#include "QDateTime"
#include "common.h"
#include "DataParser/nmea_parser.h"

namespace Ui {
class configuration_LG69TAP;
}

class configuration_LG69TAP : public QDialog
{
    Q_OBJECT

public:
    explicit configuration_LG69TAP(QWidget *parent = nullptr);
    ~configuration_LG69TAP();
signals:
    void  sendData(const char * data, unsigned int size);
public  slots:
    void updateUI(NMEA_t *msg);
private slots:
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_GC_PQTMCOLD_clicked();

    void on_pushButton_send_clicked();

    void on_GC_PQTMWARM_clicked();

    void on_GC_PQTMHOT_clicked();

    void on_GC_PQTMSRR_clicked();

    void on_GC_PQTMCFGPORT_clicked();

    void on_poll_PQTMCFGPORT_clicked();

    void on_GC_PQTMGNSSSUSPEND_clicked();

    void on_GC_PQTMGNSSRESUME_clicked();

    void on_GC_PQTMGNSSRESTART_clicked();

    void on_GC_PQTMSET_GETNMEAMSGMASK_clicked();

    void on_poll_PQTMSET_GETNMEAMSGMASK_clicked();

    void on_GC_PQTMCFGDRMODE_clicked();

    void on_poll_PQTMCFGDRMODE_clicked();

    void on_GC_PQTMCFGFWD_clicked();

    void on_poll_PQTMCFGFWD_clicked();

    void on_GC_PQTMCFGWHEELTICK_clicked();

    void on_poll_PQTMCFGWHEELTICK_clicked();

    void on_GC_PQTMCFGCAN_clicked();

    void on_poll_PQTMCFGCAN_clicked();

    void on_GC_PQTMCFGCANFILTER_clicked();

    void on_poll_PQTMCFGCANFILTER_clicked();

    void on_GC_PQTMCFGVEHDBC_clicked();

    void on_poll_PQTMCFGVEHDBC_clicked();

    void on_GC_PQTMSET_GETVEHRVAL_clicked();

    void on_poll_PQTMSET_GETVEHRVAL_clicked();

    void on_GC_PQTMSAVEPAR_clicked();

    void on_GC_PQTMRESTOREPAR_clicked();

    void on_poll_PQTMVERNO_clicked();

    void on_checkBox_RTCMOutput_clicked();

    void on_checkBox_DebugOutput_clicked();

private:
    Ui::configuration_LG69TAP *ui;
};

#endif // CONFIGURATION_LG69TAP_H

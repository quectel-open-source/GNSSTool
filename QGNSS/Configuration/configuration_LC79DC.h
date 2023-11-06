#ifndef CONFIGURATION_LC79DC_H
#define CONFIGURATION_LC79DC_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "QQueue"
#include "QByteArray"
#include "QStatusBar"
#include "QDateTime"
#include "common.h"
#include "DataParser/nmea_parser.h"

namespace Ui {
class configuration_LC79DC;
}

class configuration_LC79DC : public QDialog
{
    Q_OBJECT

public:
    explicit configuration_LC79DC(QWidget *parent = nullptr);
    ~configuration_LC79DC();

signals:
    void  sendData(const char * data, unsigned int size);
public  slots:
    void updateUI(NMEA_t *msg);

private slots:
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_pushButton_send_clicked();

    void on_GC_PQTMCOLD_clicked();

    void on_GC_PQTMWARM_clicked();

    void on_GC_PQTMHOT_clicked();

    void on_GC_PQTMSRR_clicked();

    void on_GC_PQTMSET_GETGLP_clicked();

    void on_poll_PQTMSET_GETGLP_clicked();

    void on_GC_PQTMSETBAUD_clicked();

    void on_GC_PQTMSET_GETCNST_clicked();

    void on_poll_PQTMSET_GETCNST_clicked();

    void on_GC_PQTMSET_GETL5BIAS_clicked();

    void on_poll_PQTMSET_GETL5BIAS_clicked();

    void on_GC_PQTMCFGODO_clicked();

    void on_poll_PQTMCFGODO_clicked();

    void on_poll_PQTMREQODO_clicked();

    void on_GC_PQTMRESETODO_clicked();

    void on_GC_PQTMSTARTODO_clicked();

    void on_GC_PQTMSTOPODO_clicked();

    void on_GC_PQTMCFGGEOFENCE_clicked();

    void on_poll_PQTMCFGGEOFENCE_clicked();

    void on_GC_PQTMSET_GETGEOFENCE_clicked();

    void on_poll_PQTMSET_GETGEOFENCE_clicked();

    void on_poll_PQTMREQGEOFENCE_clicked();

    void on_GC_PQTMCFGNMEAMSG_clicked();

    void on_poll_PQTMCFGNMEAMSG_clicked();

    void on_GC_PQTMCFGEAMASK_clicked();

    void on_poll_PQTMCFGEAMASK_clicked();

    void on_GC_PQTMCFGCLAMPING_clicked();

    void on_poll_PQTMCFGCLAMPING_clicked();

    void on_GC_PQTMSAVEPAR_clicked();

    void on_GC_PQTMRESTOREPAR_clicked();

    void on_GC_PQTMSETINSMSG_clicked();

private:
    Ui::configuration_LC79DC *ui;
};

#endif // CONFIGURATION_LC79DC_H

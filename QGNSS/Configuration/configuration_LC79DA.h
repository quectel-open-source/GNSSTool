#ifndef CONFIGURATION_LC79DA_H
#define CONFIGURATION_LC79DA_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "QQueue"
#include "QByteArray"
#include "QStatusBar"
#include "QDateTime"
#include "common.h"
#include "DataParser/nmea_parser.h"

namespace Ui {
class configuration_LC79DA;
}

class configuration_LC79DA : public QDialog
{
    Q_OBJECT

public:
    explicit configuration_LC79DA(QWidget *parent = nullptr);
    ~configuration_LC79DA();

signals:
    void  sendData(const char * data, unsigned int size);
public  slots:
       void updateUI(NMEA_t *msg);
private slots:
       void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

       void on_pushButton_send_clicked();

       void on_GC_PQCOLD_clicked();

       void on_GC_PQWARM_clicked();

       void on_GC_PQHOT_clicked();

       void on_GC_PQSTARTGNSS_clicked();

       void on_GC_PQSTOPGNSS_clicked();

       void on_GC_PQSRR_clicked();

       void on_GC_PQSETSLEEP_clicked();

       void on_poll_PQGETSLEEP_clicked();

       void on_GC_PQSETGLP_clicked();

       void on_poll_PQGETGLP_clicked();

       void on_GC_PQSETBAUD_clicked();

       void on_GC_PQSETCNST_clicked();

       void on_poll_PQGETCNST_clicked();

       void on_GC_PQSETL5BIAS_clicked();

       void on_poll_PQGETL5BIAS_clicked();

       void on_GC_PQSETASSTIME_clicked();

       void on_GC_PQSETASSPOS_clicked();

       void on_GC_PQSETASSGNSS_clicked();

       void on_GC_PQCFGODO_clicked();

       void on_poll_PQCFGODO_clicked();

       void on_poll_PQREQODO_clicked();

       void on_GC_PQRESETODO_clicked();

       void on_GC_PQSTARTODO_clicked();

       void on_GC_PQSTOPODO_clicked();

       void on_GC_PQCFGGEOFENCE_clicked();

       void on_poll_PQCFGGEOFENCE_clicked();

       void on_GC_PQSETGEOFENCE_clicked();

       void on_poll_PQGETGEOFENCE_clicked();

       void on_GC_PQREQGEOFENCE_clicked();

       void on_GC_PQCFGNMEAMSG_clicked();

       void on_poll_PQCFGNMEAMSG_clicked();

       void on_poll_PQCFGEAMASK_clicked();

       void on_GC_PQCFGEAMASK_clicked();

       void on_GC_PQCFGCLAMPING_clicked();

       void on_poll_PQCFGCLAMPING_clicked();

       void on_GC_PQSAVEPAR_clicked();

       void on_GC_PQRESTOREPAR_clicked();

private:
    Ui::configuration_LC79DA *ui;
};

#endif // CONFIGURATION_LC79DA_H

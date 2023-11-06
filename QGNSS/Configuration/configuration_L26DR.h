#ifndef CONFIGURATION_L26DR_H
#define CONFIGURATION_L26DR_H

#include <QDialog>
#include <QTreeWidgetItem>
#include "QQueue"
#include "QByteArray"
#include "QStatusBar"
#include "QDateTime"
#include "common.h"


namespace Ui {
class configuration_L26DR;
}

class configuration_L26DR : public QDialog
{
    Q_OBJECT

public:
    explicit configuration_L26DR(QWidget *parent = nullptr);
    ~configuration_L26DR();
    QString  tmp_Low,tmp_High,titleName;
    void getLowHighbit();
    void setReceiver_type(const quectel_gnss::QGNSS_Model &value);

signals:
    void  sendData(const char * data, unsigned int size);
public  slots:
       void updateUI(QByteArray msg);
private slots:
    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

    void on_GC_PSTMINITGPS_clicked();

    void on_pushButton_send_clicked();

    void on_GC_PSTMINITTIME_clicked();

    void on_GC_PSTMCLREPHS_clicked();

    void on_GC_PSTMDUMPEPHEMS_clicked();

    void on_GC2_PSTMCLRALMS_clicked();

    void on_GC2_PSTMDUMPALMANAC_clicked();

    void on_GC_PSTMCOLD_clicked();

    void on_GC_PSTMWARM_clicked();

    void on_GC_PSTMHOT_clicked();

    void on_GC_PSTMSRR_clicked();

    void on_GC_PSTMSBASONOFF_clicked();

    void on_GC_PSTMSBASSERVICE_clicked();

    void on_poll_PSTMGETRTCTIME_clicked();

    void on_GC2_PSTMSETCONSTMASK_clicked();

    void on_GC_PSTMCFGCONST_clicked();

    void on_poll_PPS_IF_PULSE_DATA_CMD_clicked();

    void on_poll_PPS_IF_TIMING_DATA_CMD_clicked();

    void on_poll_PPS_POSITION_HOLD_clicked();

    void on_poll_PPS_IF_TRAIM_CMD_clicked();

    void on_poll_PPS_IF_TRAIM_USED_CMD_clicked();

    void on_poll_PPS_IF_TRAIM_RES_CMD_clicked();

    void on_poll_PPS_IF_TRAIM_REMOVED_CMD_clicked();

    void on_GC_PPS_IF_ON_OFF_CMD_clicked();

    void on_GC_PPS_IF_OUT_MODE_CMD_clicked();

    void on_GC_PPS_IF_REFERENCE_TIME_CMD_clicked();

    void on_GC_PPS_IF_PULSE_DELAY_CMD_clicked();

    void on_GC_PPS_IF_CONSTELLATION_RF_DELAY_CMD_clicked();

    void on_GC_PPS_IF_PULSE_DURATION_CMD_clicked();

    void on_GC_PPS_IF_PULSE_POLARITY_CMD_clicked();

    void on_GC_Set_PPS_IF_PULSE_DATA_CMD_clicked();

    void on_GC_PPS_IF_FIX_CONDITION_CMD_clicked();

    void on_GC_PPS_IF_SAT_TRHESHOLD_CMD_clicked();

    void on_GC_PPS_IF_ELEVATION_MASK_CMD_clicked();

    void on_GC_PPS_IF_CONSTELLATION_MASK_CMD_clicked();

    void on_GC_Setting_PPS_IF_TIMING_DATA_CMD_clicked();

    void on_GC_Set_PPS_IF_POSITION_HOLD_DATA_CMD_clicked();

    void on_GC_PPS_IF_AUTO_HOLD_SAMPLES_CMD_clicked();

    void on_GC_Setting_PPS_IF_TRAIM_CMD_clicked();

    void on_GC_PSTMFORCESTANDBY_clicked();

//    void on_poll_PSTMGEOFENCEREQ_clicked();

    void on_GC_PSTMODOSTART_clicked();

    void on_GC_PSTMODOSTOP_clicked();

    void on_poll_PSTMODOREQ_clicked();

    void on_GC_PSTMODORESET_clicked();

    void on_GC_PSTMCFGPORT_clicked();

    void on_GC_PSTMCFGTDATA_clicked();

    void on_GC_PSTMCFGMSGL_clicked();

    void on_GC_PSTMCFGAGPS_clicked();

    void on_GC_PSTMCFGAJM_clicked();

    void on_GC_PSTMCFGODO_clicked();

    void on_GC_PSTMCFGGEOFENCE_clicked();

    void on_GC_PSTMIMUSELFTESTCMD_clicked();

    void on_GC_PSTMCFGGEOCIR_clicked();

    void on_GC_PSTMSETTHTRK_clicked();

    void on_GC_PSTMSETTHPOS_clicked();

    void on_poll_PSTMGETPAR_1500_clicked();

    void on_GC_PSTMSETPAR_1122_clicked();

    void on_poll_PSTMGETPAR_1122_clicked();

    void on_GC_PSTMSAVEPAR_clicked();

    void on_GC_PSTMRESTOREPAR_clicked();

    void on_poll_PSTMGEOFENCEREQ_clicked();

private:
    Ui::configuration_L26DR *ui;
    quectel_gnss::QGNSS_Model receiver_type = quectel_gnss::QGNSS_Model::L26ADR;

};

#endif // CONFIGURATION_L26DR_H

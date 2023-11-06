#ifndef CONFIGURATION_P1_H
#define CONFIGURATION_P1_H

#define D_CRC_OFFSET (8)

#include <QDialog>
#include <QStatusBar>
#include <QTreeWidgetItem>

#include "common.h"
//#include "p1_binary_msg.h"
#include "messages_p1/defs.h"
#include "messages_p1/configuration.h"
#include "messages_p1/control.h"
namespace Ui {
class configuration_P1;
}

//using namespace point_one::fusion_engine::messages;

class configuration_P1 : public QDialog
{
    Q_OBJECT

public:
    explicit configuration_P1(QWidget *parent = nullptr);
    ~configuration_P1();
    void setModel_type(const quectel_gnss::QGNSS_Model &value);

signals:
    void sendData(QByteArray cmd);

public  slots:
       void updateUI(QByteArray msg);
private slots:
       void on_pushButton_send_clicked();

       void on_GC_baudrate_clicked();

       void on_GC_DeviceOrientation_clicked();
       void on_PL_DevLocation_clicked();

       void on_GC_DeviceLocation_clicked();
       void on_PL_DevOrientation_clicked();

       void on_GC_AntLocation_clicked();
       void on_PL_AntLocation_clicked();

       void on_GC_OutputLocation_clicked();
       void on_PL_OutputLocation_clicked();

       void on_GC_SaveSettings_clicked();

       void on_groupBox_cunstomReset_clicked(bool checked);

       void on_groupBox_specialReset_clicked(bool checked);

       void on_GC_reset_clicked();

       void on_GC_vehicleDetails_clicked();

       void on_GC_wheelMeasurement_clicked();

       void on_GC_setOutput_clicked();

       void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);

       void on_GC_13001_clicked();

       void on_GC_shutdown_clicked();

       void on_GC_HWTC_clicked();

       void on_GC_UDE_clicked();

       void on_GC_WDogEnable_clicked();

private:
    Ui::configuration_P1 *ui;
    quectel_gnss::QGNSS_Model model_type = quectel_gnss::QGNSS_Model::LG69TAQ;
    QStatusBar *statusBar;
    point_one::fusion_engine::messages::MessageHeader m_Header;
    point_one::fusion_engine::messages::SetConfigMessage m_setConfigPayload;

    QByteArray GenerateCMD(QByteArray payload);
    uint32_t _crc32(const char * change_data, uint32_t lenght);
};

#endif // CONFIGURATION_P1_H

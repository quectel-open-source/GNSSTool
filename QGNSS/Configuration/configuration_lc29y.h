#ifndef CONFIGURATION_LC29Y_H
#define CONFIGURATION_LC29Y_H

#define GPS_L1          0x01
#define GLONASS_G1      0x02
#define BeiDou_B1I      0x04
#define Galileo_E1      0x10
#define QZSS_L1         0x20
#define GPS_L1C         0x100
#define GPS_L5          0x200
#define BeiDou_B2a      0x8000
#define Galileo_E5a     0x100000
#define QZSS_L5         0x4000000


#include <QDialog>
#include <QStatusBar>

namespace Ui {
class configuration_LC29Y;
}

class configuration_LC29Y : public QDialog
{
    Q_OBJECT

public:
    explicit configuration_LC29Y(QWidget *parent = nullptr);
    ~configuration_LC29Y();
    void addCheckSum(QByteArray &cmdload);

private slots:
    void on_pushButton_PRT_GC_clicked();

    void on_pushButton_send_clicked();

    void on_pushButton_PRT_GC_2_clicked();

    void on_pushButton_PRT_GC_NAVSAT_clicked();

    void on_pushButton_PRT_GC_CFG_SIMPLERST_clicked();

private:
    Ui::configuration_LC29Y *ui;
    QStatusBar *statusBar;
    void showMSG(QString msg);
};

#endif // CONFIGURATION_LC29Y_H

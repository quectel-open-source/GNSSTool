#ifndef CONFIGURATION_L76K_H
#define CONFIGURATION_L76K_H

#include <QDialog>
#include "QStatusBar"

namespace Ui {
class configuration_L76K;
}

class configuration_L76K : public QDialog
{
    Q_OBJECT

public:
    explicit configuration_L76K(QWidget *parent = nullptr);
    ~configuration_L76K();

    static void addChecksum(QByteArray &cmd);

private slots:
    void on_pushButton_send_clicked();

    void on_pushButton_PCAS_1_clicked();

    void on_pushButton_PCAS_2_clicked();

    void on_pushButton_PCAS_3_clicked();

    void on_pushButton_PCAS_4_clicked();

    void on_pushButton_PCAS_10_clicked();

    void on_pushButton_CFG_PRT_clicked();

    void on_pushButton_CFG_PRT_poll_clicked();

    void on_pushButton_CFG_MSG_clicked();

    void on_pushButton_CFG_MSG_poll_clicked();

    void on_pushButton_CFG_RST_clicked();

    void on_pushButton_CFG_RATE_clicked();

    void on_pushButton_CFG_RATE_poll_clicked();

private:
    Ui::configuration_L76K *ui;
    QStatusBar *statusBar;
};

#endif // CONFIGURATION_L76K_H

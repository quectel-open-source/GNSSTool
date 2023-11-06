#ifndef TTFF_TESTDIALOG_H
#define TTFF_TESTDIALOG_H

#include <QDialog>
//#include <QChartView>
//#include <QLineSeries>
//#include <QValueAxis>
//#include <QScatterSeries>
#include "DataParser/nmea_parser.h"
#include "mainwindow.h"

//QT_CHARTS_USE_NAMESPACE
using namespace quectel_gnss;

namespace Ui {
class TTFF_TestDialog;
}

class TTFF_TestDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TTFF_TestDialog(QWidget *parent = nullptr);
    ~TTFF_TestDialog();

    void setSolution_data(Solution_t *value);

signals:
    void openSetAGNSS_Dialog();
    void sendAgnssData();

private slots:
    void on_pushButton_Run_clicked(bool checked);

    void check_postion_timer();

    void on_radioButton_cold_clicked(bool checked);

    void on_radioButton_warm_clicked(bool checked);

    void on_radioButton_hot_clicked(bool checked);

    void on_radioButton_full_clicked(bool checked);

    void on_pushButton_SetAGNSS_clicked();

private:
    Ui::TTFF_TestDialog *ui;

    bool sendRestart();
    void setEnabledUI(bool en);
    void clearUI();
    QFile *ttffLogFile;
    QTimer *fTimer;
    QTime fTimeCounter;
    enum RestartType rType=RestartType::Cold;
    uint32_t NumOfTests;
    int Timeout_ms;
    int maxFixTime = 0;
    int minFixTime = 0;
    int sumFixTime = 0;
    int invalidFixTime = 0;
    Solution_t *solution_data = nullptr;
    int step = 0;
    int TestedCount = 0;
};

#endif // TTFF_TESTDIALOG_H

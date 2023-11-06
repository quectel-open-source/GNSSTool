#ifndef MSG_DIALOG_H
#define MSG_DIALOG_H

#include <QDialog>
#include <QTreeWidgetItem>
#include <QStandardItemModel>
#include <QDebug>
#include <QRegularExpression>
#include <QTime>
#include <QTimer>
#include <QDateTime>
#include <map>
#include <regex>
#include <QTextCodec>
#include "DataParser/nmea_parser.h"
#include "qcustomplot.h"

#define PLOTCOUNT 100
#define PLOTFALG  1

namespace Ui {
class Msg_Dialog;
}

class Msg_Dialog : public QDialog
{
    Q_OBJECT

public:
    explicit Msg_Dialog(QWidget *parent = nullptr);
    ~Msg_Dialog();
    void update_type999();
    void update_type999_sub1();
    void update_type999_sub2();
    void update_type999_sub3();
    void update_type999_sub19();
    void update_type999_sub21();
    void update_type1005();
    void update_type1006();
    void update_type1029();
    //===========================
    void textDataUpdateAcc();
    void showPlotAcc();
    void clearAllData();


    QVector<double> xAcc    ;
    QVector<double> xUTC    ;
    QVector<double> yHeading;
    QVector<double> yPitch  ;
    QVector<double> yRoll   ;

    QMap<double,double>TimeQMap;


    QTimer *timerAcc = nullptr;
    QCPItemTracer * tracerHeading;
    QCPItemTracer * tracerPitch;
    QCPItemTracer * tracerRoll;
    QList<QColor> plotColor;
    bool flagAcc = false;

//    QMenu *pAccMenu;
//    QAction *acc_Clear ;
//    QAction *acc_Reset ;
//    QAction *acc_Datacomparison;




public  slots:
    void getParseDataLC02H(NMEA_t msg);
    void timeToRePlotAcc();
    void accMoveMouseEventH(QMouseEvent *event);
    void accMoveMouseEventP(QMouseEvent *event);
    void accMoveMouseEventR(QMouseEvent *event);
//    void mouseReleaseSlotsAcc();
//    void OnAccDatarComparisonAction();
//    void OnClearActionAcc();
//    void OnResetActionAcc();
    //=============================

private slots:
    void timerUpdate();

    void on_treeWidget_currentItemChanged(QTreeWidgetItem *current, QTreeWidgetItem *previous);


//    void on_stackedWidget_currentChanged(int arg1);

    void on_checkBoxHeading_clicked();

    void on_checkBoxPitch_clicked();

    void on_checkBoxRoll_clicked();

    void on_pushButtonReset_clicked();

    void on_pushButtonStartPause_clicked();

private:
    Ui::Msg_Dialog *ui;
    QMap<QString,QTreeWidgetItem*> itemMap;
    QString resp_Str[3] = {"OK","Error","RFU"};
    QStringList normalHeader;
    QStringList respHeader;
    QStandardItemModel* model;

};

#endif // MSG_DIALOG_H

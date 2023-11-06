#ifndef LOGPLOT_MAINWINDOW_H
#define LOGPLOT_MAINWINDOW_H

#include "qcustomplot.h"
#include "qwaiting.h"
#include "aboutdialog.h"
#include "qnmea.h"
#include "nmea_dt.h"
#include "qlog_data_center.h"
#include "infodialog.h"
#include "FilterOptionsDialog.h"
#include <QMainWindow>
#include <QHostInfo>
#include <QMap>
#include <QRegExp>
#include <QDebug>
#include <QRegularExpression>
#include <QtConcurrent>
#include <QTime>
#include <QTimer>
#include <QDateTime>
#include <map>
#include <regex>
#include <QTextCodec>

#define LOG_VERSION "QGNSSLog_V1.2.3"

typedef struct {
    int      nMIN;
    int      nMAX;
    int      nCount;
    double   dAverage;

}Max_Min_AVG_t;

typedef struct {

    int             serialNumber;
    int             PositioningState;
    double          dUTC;
    double          CN0_Average;
    int             CN0_Max;
    int             CN0_Min;
    unsigned short  TotalNumSatView;
    unsigned short  TotalNumSatUsed;

}Frame_Data;

QT_BEGIN_NAMESPACE
namespace Ui { class logPlot_MainWindow; }
QT_END_NAMESPACE

class logPlot_MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    logPlot_MainWindow(QWidget *parent = nullptr);
    ~logPlot_MainWindow();

    static void get_log_data_cb(QLOG_Data *logdata,void * userd);
    static void get_log_status_cb(async_read_status *status,void *userdata);

    QWaiting *pAllQwait = nullptr;
    QString  fileName_save;
    QCPItemTracer * tracer;
    QMenu *pMenu;
    QAction *act_Clear ;
    QAction *act_Reset ;
    QAction *act_Datacomparison;
    /**
     * @brief for storing curve data
     */
    QVector<double> all_x;              //x-axis data
    QVector<double> x_VU;               //View and Used x-axis data
    QVector<double> y_VU;               //View and Used y-axis data
    QList<QVector<double>> x_dataList;  //x-axis data set
    QVector<double> AlarmValue_y;       //Guard value may be eliminated?
    QVector<double> PositionState_x;    //PositionState  x-axis data
    QVector<double> PositionState_y;    //PositionState  y-axis data
    QList<QVector<double>> y_dataList;  //y-axis data set
    QList<QColor> plotColor;            //Collection of all curve colors
    QList<QString> graphName;           //Collection of all curve name
    bool plotFlag  = false;             //Flag bits for scatter plots and line plots
    void firstOpenHelpWrite();
    int firstOpenHelpRead();
    QString firstOpenHelpReadTwo();
    QPushButton* agreeBut;
    QPushButton* disagreeBut;

    QMap<double,double>PositionStateMap;
    QMap<double,double>TimeQMap;

    QList<QMap<double,double>>qMListmap;

    FilterOptionsDialog *FilterOptions;



    /**
     * @brief setPlotColorList:PlotColor adds all frequency curve colors
     */
    void setPlotColorList();

    /**
     * @brief setSlotsAssemble:Slot function of all frequency check boxes
     */
    void setSlotsAssemble();
    /**
     * @brief plotCrete
     * @param id
     * @param color
     * @param x_data
     * @param y_data
     * @param graphName
     */
    void plotCrete(int id, QColor color, QVector<double> x_data, QVector<double> y_data, QString graphName);
    /**
     * @brief plotData:All graph drawing handlers
     */
    void plotData();
    /**
     * @brief getXDataTime
     * @param tDate
     * @param dUTC
     * @return
     */
    double getXDataTime(NMEA_Base::NMEA_Data tDate, NMEA_Base::NMEA_Time dUTC);

    /**
     * @brief showPlotData:Acquisition of graph data
     */
    void showPlotData();
    /**
     * @brief  show qwt Curve Diagram
     */
    void showPlot();

    /**
     * @brief getCNoAVGMaxMin
     * @param id
     * @param signalChannel
     * @param gpsID
     * @param avgCNo
     * @param maxCNo
     * @param minCNo
     */
    void getCNoAVGMaxMin(QCheckBox *checkBox, int id, QLabel *signalChannel, QString gpsID, QLabel *avgCNo, QLabel *maxCNo, QLabel *minCNo);

    /**
     * @brief m_MMA:struct object
     */
    Max_Min_AVG_t m_CNoMMA;
    /**
     * @brief mmaCNoList
     */
    QList<Max_Min_AVG_t>   mmaCNoList;

    Frame_Data frameData;
    QList<Frame_Data> frameDataList;
    QList<QList<Frame_Data>> frameDataListALL;


    /**
     * @brief getMaxMinAVG:Get MAX Min AVG in all CNo values
     */
    void getMaxMinAVG();

    /**
     * @brief curveShowOrHide : Show and hide control curves
     * @param checkBox        : control object name
     * @param id              : Curve number
     * @param signalChannel   : Signal Channel
     * @param avgCNo          : AVG lable data
     * @param maxCNo          : MAX lable data
     * @param minCNo          : MIN lable data
     */
    void curveShowOrHide(QCheckBox *checkBox, int id, QLabel *signalChannel, QLabel *avgCNo, QLabel *maxCNo, QLabel *minCNo);

    /**
     * @brief getCheckboxTickStatus
     * @param checkbox
     * @param y_data
     */
    void getCheckboxTickStatus(QCheckBox *checkbox, QVector<double>y_data, QLabel *label);

    /**
     * @brief checkboxTickControl : Show checkboxes for bands present in log and ticked.
     */
    void checkboxTickControl();

    /**
     * @brief setCheckboxStatus : Sets the response function of the frequency band check box
     */
    void setCheckboxStatus();

    /**
     * @brief getCopyText
     * @param signalChannel
     * @param avgCNo
     * @param maxCNo
     * @param minCNo
     * @return
     */
    QString getCopyText(QCheckBox *checkBox, QLabel *signalChannel, QLabel *avgCNo, QLabel *maxCNo, QLabel *minCNo);

    void getLogData(QLOG_Data *logdata);
    void getLogStatus(async_read_status *status);
    void setStyleAllCheckBox();

signals:

    void logDataChange(QLOG_Data *logdata);
    void logStatusChange(async_read_status *status);

public  slots:
    /**
     * @brief buttonClicked
     * @param butClicked
     */
    void buttonClicked(QAbstractButton * butClicked);
    /**
     * @brief showTime:Real-time update time and show
     */
    void showTime();
    /**
     * @brief mouseReleaseSlots
     */
    void mouseReleaseSlots();
    /**
     * @brief OnClearAction
     */
    void OnClearAction();
    /**
     * @brief OnResetAction
     */
    void OnResetAction();
    /**
     * @brief OnCopyAction
     */
    void OnCopyAction();

    void OnDatarComparisonAction();

    void myMoveMouseEvent(QMouseEvent *event);


private slots:
    void on_radioButton_CNo_clicked();
    void on_radioButton_View_clicked();
    void on_radioButton_Used_clicked();
    void on_checkBox_GPSL1CA_clicked();
    void on_checkBox_GPSL5Q_clicked();
    void on_checkBox_GLG1CA_clicked();
    void on_checkBox_GaE5a_clicked();
    void on_checkBox_GaL1A_clicked();
    void on_checkBox_BDSB1I_clicked();
    void on_checkBox_BDSB2a_clicked();
    void on_checkBox_GQL1CA_clicked();
    void on_checkBox_GQL5Q_clicked();
    void on_checkBox_GIL5SPS_clicked();
    void on_checkBox_ALL_clicked();
    void on_actionAbout_QGNSSLog_triggered();
    void on_actionOpen_File_triggered();
    void on_checkBox_GPALL_clicked();
    void on_checkBox_GPSL1PY_clicked();
    void on_checkBox_GPSL1M_clicked();
    void on_checkBox_GPSL2PY_clicked();
    void on_checkBox_GPSL2CM_clicked();
    void on_checkBox_GPSL2CL_clicked();
    void on_checkBox_GPSL5I_clicked();
    void on_checkBox_GLALL_clicked();
    void on_checkBox_GLG1P_clicked();
    void on_checkBox_GLG2CA_clicked();
    void on_checkBox_GLG2P_clicked();
    void on_checkBox_GAALL_clicked();
    void on_checkBox_GaE5b_clicked();
    void on_checkBox_GaE5ab_clicked();
    void on_checkBox_GaE6A_clicked();
    void on_checkBox_GaE6BC_clicked();
    void on_checkBox_GaL1BC_clicked();
    void on_checkBox_GBALL_clicked();
    void on_checkBox_BDSB1Q_clicked();
    void on_checkBox_BDSB1C_clicked();
    void on_checkBox_BDSB1A_clicked();
    void on_checkBox_BDSB2b_clicked();
    void on_checkBox_BDSB2ab_clicked();
    void on_checkBox_BDSB3I_clicked();
    void on_checkBox_BDSB3Q_clicked();
    void on_checkBox_BDSB3A_clicked();
    void on_checkBox_BDSB2I_clicked();
    void on_checkBox_BDSB2Q_clicked();
    void on_checkBox_GQALL_clicked();
    void on_checkBox_GQL1CD_clicked();
    void on_checkBox_GQL1CP_clicked();
    void on_checkBox_GQLIS_clicked();
    void on_checkBox_GQL2CM_clicked();
    void on_checkBox_GQL2CL_clicked();
    void on_checkBox_GQL5I_clicked();
    void on_checkBox_GQL6E_clicked();
    void on_checkBox_GQL6D_clicked();
    void on_checkBox_GIALL_clicked();
    void on_checkBox_GISSPS_clicked();
    void on_checkBox_GIL5RS_clicked();
    void on_checkBox_GISRS_clicked();
    void on_checkBox_GIL1SPS_clicked();
    void on_checkBox_WAAS_clicked();
    void on_checkBox_SDCM_clicked();
    void on_checkBox_EGNOS_clicked();
    void on_checkBox_BDSBAS_clicked();
    void on_checkBox_MSAS_clicked();
    void on_checkBox_GAGAN_clicked();
    void on_actionline_Chart_triggered();
    void on_actionScatter_Plot_triggered();
    void on_actionPDF_triggered();
    void on_actionBMP_triggered();
    void on_actionJPG_triggered();
    void on_actionPNG_triggered();
    void on_checkBox_BlackLine_clicked();
    void on_actionPDF_2_triggered();
    void on_actionBMP_2_triggered();
    void on_actionJPG_2_triggered();
    void on_actionPNG_2_triggered();
    void on_checkBox_GrandMean_clicked();
    void on_actionHelp_triggered();   
    void on_checkBox_Status_clicked();

    void on_actionFilter_options_triggered();

protected:
    void mouseReleaseEvent(QMouseEvent * event);
    void keyReleaseEvent(QKeyEvent *event);
    void dragEnterEvent(QDragEnterEvent* event);
    void dropEvent(QDropEvent* event);
    void wheelEvent(QWheelEvent *event);

private:
    Ui::logPlot_MainWindow *ui;

};
#endif // LOGPLOT_MAINWINDOW_H

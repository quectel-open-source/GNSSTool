#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "qgnss_flashtool.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "spdlog/sinks/daily_file_sink.h"
#include "playercontrol.h"
#include "commanddialog.h"
//#include "nmea2kml.h"
#include "Configuration/configuration_lc29d.h"
#include "Configuration/configuration_L26DR.h"
#include "Configuration/configuration_LC79DA.h"
#include "Configuration/configuration_LC79DC.h"
#include "Configuration/configuration_LG69TAS.h"
#include "Configuration/configuration_LG69TAP.h"
//#include "status_led.h"
#include "AGNSS/FtpClientDialog.h"
#include "signallevelwindow.h"
#include "deviation_map.h"
#include "SkyViewWidget.h"
#include "mapwebviewwindow.h"
#include "DataParser/rawdataparser.h"
#include "DataParser/nmea_parser.h"
#include "DataParser/mtk_bin_parser.h"
#include "RTKLib/rtklib.h"
#include "DataParser/rtcm3parser.h"
#include "dr_dialog.h"
#include "ntrip_clientdialog.h"
#include "preferences_dialog.h"
#include "device_info.h"
#include "msg_dialog.h"
#include <QMdiSubWindow>
#include <QProcess>
#include <QWebEngineView>
#include <QSerialPort>
#include <QMainWindow>

QT_BEGIN_NAMESPACE

namespace Ui
{
    class MainWindow;
}

QT_END_NAMESPACE

class Console;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setSubwindowDefaultLayout();
    void updateDataView(void);
    void clearAllData();
    void sendResetCMD(int cmd);
    int sendAndWaitForACK(QByteArray cmd, const unsigned int &nACK);
    device_info::device_info_t dev_info;
    QSerialPort *m_serial = nullptr;
    PlayerControl *m_playerControl = nullptr;
    QString portName;
    RawDataParser *rawDataParser = nullptr;
    NMEA_t NMEA_Data;
    NMEA_t *nmea_t;
    NMEA_Parser *m_NMEA_Parser;
    MTKbin_t MTKbin_Data;
    mtk_bin_parser *m_mtk_bin_parser;
    int modelIndex = quectel_gnss::QGNSS_Model::NONE;
    QByteArray ttff_cmd_info[5];
    double altitude = 0;

    QCheckBox *DTR;
    QCheckBox *RTS;


protected:
    void closeEvent(QCloseEvent *event) override;

private:
    void initActionsConnections();
    void update_online_map_view(bool judge = true);
    void showStatusMessage(const QString &message);
    void readSettings();
    void writeSettings();
    bool enableLogging();
    void Analyze(QByteArray &data);
    uint32_t getCurrentInstanceNumber();
    Ui::MainWindow *ui = nullptr;
    QLabel *m_status = nullptr;
    NTRIP_ClientDialog *m_ntripclientdialog = nullptr;
    FtpClientDialog *m_FtpClientDialog = nullptr;
    QMdiSubWindow *m_signallevelwindow = nullptr;
    QMdiSubWindow *m_binConsolewindow = nullptr;
    QMdiSubWindow *m_textConsoleWindow = nullptr;
    QMdiSubWindow *m_skyViewWindow = nullptr;
    QMdiSubWindow *m_deviationMapWindow = nullptr;
    QMdiSubWindow *m_mapWebWindow = nullptr;
    SignalLevelWindow *m_signallevelWidget = nullptr;
    MapWebViewWindow *m_mapWebWidget = nullptr;
    Deviation_Map *m_deviationMapWidget = nullptr;
    SkyViewWidget *m_skyViewWidget = nullptr;
    ConfigurationLC29D *configurationLC29D = nullptr;
    Preferences_Dialog *preferences;
    double oMapCurlon = 0;
    double oMapCurlat = 0;
//    uint32_t refreshUI_Flag = 0;

    // save received data to file
    bool m_storeStatus = 0; // store status
    QFile *m_storeFile = nullptr;

    // replay from file
    int64_t m_replayFileSize = 0;
    QFile m_replayFile;
    QTimer m_replayFile_Timer1; /* timer */

    QMap<int, QString> QualityIndicator_m = {{0, "Invalid"},
                                             {1, "GPS SPS Mode"},
                                             {2, "DGNSS"},
                                             {3, "PPS"},
                                             {4, "Fixed RTK"},
                                             {5, "Float RTK"},
                                             {6, "DR"},
                                             {7, "Manual Input"},
                                             {8, "Simulator"}};
    QMap<char, QString> FixMode_m = {{'1', "-"},
                                     {'2', "2D"},
                                     {'3', "3D"}};
    QMap<char, QString> DR_Type = {{0, "DR not ready"},
                                   {1, "DR not ready(1)"},
                                   {2, "GNSS + DR mode"},
                                   {3, "DR only mode"}};

    QTimer *displayTimer;
    QString spdlogname="stroespdlog";
signals:
    void ttffTestfixedIndication();
    void clearData();
    void displayDumpData(QByteArray);
    void displayHexData(QByteArray);
    void displayTextData(QByteArray);
    void parseData(QByteArray);
    void parseData_L26DR(QByteArray);

public slots:
    void ReadDataFromSerial();
    void sendData(const QByteArray &data);
    void sendData(const char *data, unsigned int size);
    void send_GNSS_cold_start();
    void send_GNSS_warm_start();
    void send_GNSS_hot_start();
    void delayTask(void);

    void clickDTR();
    void clickRTS();

    bool createSpdlog();
private slots:
    void clearUI(void);
    void updateUI(void);
    void loadModelConfig();
    void handleError(QSerialPort::SerialPortError error);
    void updateFirmware();
    void OpenQGNSS_Log();
    void ShowLogFilesInExplorer();
    void replayFromFile();
    void ReadDataFromFile(QByteArray data);
    void turn_Cmd_console_onoff();
    void turn_Configuration_view_onoff();
    void turn_Static_TTFF_testing_Dialog_onoff();
    void turn_DR_Information_Dialog_onoff();
    void turn_Position_window_onoff();
    void turn_Sky_Map_window_onoff();
    void on_actionTianDi_map_triggered();
    void on_actionOSM_map_triggered();
    void on_actionGNSS_signal_triggered();
    void on_actionBin_console_triggered();
    void on_actionTextConsole_triggered();
    void on_actionOnlineMap_triggered();
    void on_actionTile_triggered();
    void on_actionCascade_triggered();
    void on_actionClose_All_triggered();
    void on_actionConnect_toggled(bool checked);
    void on_actionAbout_triggered();
    void on_actionNTRIP_Client_triggered();
    void on_actionCheck_for_updates_triggered();
    void on_actionMessages_View_triggered();
    void on_actionFullColdStart_triggered();
    void on_action_Assistant_GNSS_Offline_triggered();
    void on_action_Assistant_GNSS_Online_triggered();
    void on_actionRawDataConsole_triggered();
    void on_actionPreferences_triggered();
    void on_actionDeviceInfo_triggered();
    void on_actionConvert_KML_triggered();
    void on_actionCoordinate_Converter_triggered();
    void on_actionDock_Tools_triggered();
    void on_actionBase_Station_triggered();
};

extern MainWindow *mainForm;
#endif // MAINWINDOW_H

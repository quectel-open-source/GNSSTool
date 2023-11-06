#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "about.h"
#include "DataParser/rtcm3parser.h"
#include "DataParser/casic_parser.h"
#include "ttff_testdialog.h"
#include "console.h"
#include "surveyview.h"
#include "QSimpleUpdater.h"
#include "Configuration/configuration_l76k.h"
#include "Configuration/configuration_lg69taa.h"
#include "Configuration/configuration_lc29y.h"
#include "Configuration/configuration_p1.h"
#include "Configuration/config_lc26g.h"
#include "AGNSS/agnss_l76k_dialog.h"
#include "AGNSS/agnss_lc76f_dialog.h"
#include "AGNSS/agnss_lc29h_dialog.h"
#include "AGNSS/agnss_lc79d_dialog.h"
#include "AGNSS/agnss_lc29h_hostdialog.h"
#include "AGNSS/agnss_lc29y_online_dialog.h"
#include "msg_dialog.h"
#include "JsonOperate.h"
#include "convert_kml.h"
#include "CoordinateConverterDialog.h"
#include "AGNSS/hd8120_offline_dialog.h"

#include <QMessageBox>
#include <QDesktopServices>
#include <QProgressBar>

#define D_BINARY_WINDOW_NAME "Binary data"
#define D_RAW_WINDOW_NAME "Unparsed data"
#define D_TEXT_WINDOW_NAME "Text data"
#define D_DEVIATION_WINDOW_NAME "Deviation Map"
#define D_SIGNAL_WINDOW_NAME "Signal Level"

extern QQueue<QByteArray> g_rtcm3_queue;
MainWindow *mainForm;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          m_serial(new QSerialPort(this)),
                                          rawDataParser(new RawDataParser(this)),
                                          ui(new Ui::MainWindow),
                                          m_status(new QLabel),
                                          m_FtpClientDialog(new FtpClientDialog((this)))
{
    ui->setupUi(this);
    mainForm = this;
    /*Sky view widget and windows*/
    m_skyViewWidget = new SkyViewWidget;
    m_skyViewWindow = ui->mdiArea->addSubWindow(m_skyViewWidget);
    m_skyViewWindow->setAttribute(Qt::WA_DeleteOnClose, false);
    m_skyViewWindow->hide();
    /*mapWeb view widget and windows*/
#ifdef OPEN_ONLINEMAP
    m_mapWebWidget = new MapWebViewWindow(this);
    m_mapWebWindow = ui->mdiArea->addSubWindow(m_mapWebWidget);
    m_mapWebWindow->setAttribute(Qt::WA_DeleteOnClose, false);
    m_mapWebWindow->hide();
#else
    m_ui->menuOn_Map_Switch->setEnabled(false);
    m_ui->actionOnlineMap->setVisible(false);
#endif
    /*signallevel view widget and windows*/
    m_signallevelWidget = new SignalLevelWindow(this);
    m_signallevelwindow = ui->mdiArea->addSubWindow(m_signallevelWidget);
    m_signallevelwindow->setAttribute(Qt::WA_DeleteOnClose, false);
    m_signallevelwindow->hide();

    configurationLC29D = new ConfigurationLC29D(this);
    connect(configurationLC29D, SIGNAL(sendData(const char *, unsigned int)), this, SLOT(sendData(const char *, unsigned int)));

//    ui->statusBar->addWidget(m_status);

    /* Preferences_Dialog */
    preferences = new Preferences_Dialog(this);
    connect(preferences, SIGNAL(applyMapSettings()), m_mapWebWidget, SLOT(applySettings()));
    connect(preferences, SIGNAL(applySettings()), rawDataParser, SLOT(updeteSettings()));

    /* PlayControl_Dialog */
    m_playerControl = new PlayerControl(this);
    QObject::connect(m_playerControl, &QDialog::rejected, [=]()
                     {
        ui->actionConnect->setEnabled(true);
        ui->actionReplayFile->setEnabled(true); });
    connect(m_playerControl, SIGNAL(sendData2Parser(QByteArray)), this, SLOT(ReadDataFromFile(QByteArray)));

    initActionsConnections();
    m_NMEA_Parser = new NMEA_Parser(nullptr, &NMEA_Data);
    m_mtk_bin_parser = new mtk_bin_parser(&MTKbin_Data);
    displayTimer = new QTimer(this);
    displayTimer->setInterval(360);
    displayTimer->stop();
    connect(displayTimer, SIGNAL(timeout()), this, SLOT(updateUI()));
    displayTimer->start();
    m_skyViewWidget->setGsv_t(&NMEA_Data.GSV_Data);
    m_skyViewWidget->setSol(&NMEA_Data.Solution_Data);
    m_FtpClientDialog->setSolution_data(&NMEA_Data.Solution_Data);
    initRTCM3Parser();

    /* Read Settings file */
    readSettings();

    //DTR RTS
    DTR = new QCheckBox;
    RTS = new QCheckBox;
    DTR->setText("DTR");
    RTS->setText("RTS");
    ui->statusBar->addWidget(DTR);
    ui->statusBar->addWidget(RTS);
    ui->statusBar->addWidget(m_status);
    connect(DTR,&QCheckBox::clicked,this,&MainWindow::clickDTR);
    connect(RTS,&QCheckBox::clicked,this,&MainWindow::clickRTS);
}
//! [3]

MainWindow::~MainWindow()
{
    //    writeSettings();
    if (m_storeFile != nullptr && m_storeFile->isOpen())
        m_storeFile->close();
    delete m_storeFile;
    delete m_ntripclientdialog;
    delete preferences;
    delete ui;
    qInfo() << "Quit QGNSS.";
}

//! [4]

void MainWindow::loadModelConfig()
{
    QFile loadFile(QStringLiteral("./ModelInfo.json"));
    qDebug() << "json file path:" << QDir::currentPath();
    if (!loadFile.open(QIODevice::ReadOnly))
    {
        qWarning("Couldn't open save file.");
    }
    QByteArray saveData = loadFile.readAll();
    loadFile.close();
    QJsonDocument loadDoc(QJsonDocument::fromJson(saveData));
    QJsonObject json(loadDoc.object());
    if (json.contains("Model") && json["Model"].isArray())
    {
        QJsonArray modelArray = json["Model"].toArray();
        foreach (QJsonValue iJson, modelArray)
        {
            QJsonObject typeObject = iJson.toObject();
            if (typeObject["name"].toString() == dev_info.stringModelType)
            {
                //                if (modelObject["Type"].isArray())
                //                {
                //                    QJsonArray typeArray = modelObject["Type"].toArray();
                //                    foreach (QJsonValue jJson, typeArray)
                //                    {
                //                        QJsonObject typeObject = jJson.toObject();
                //                        if (typeObject["name"].toString() == m_settings->m_module_subType)
                //                        {
                ui->actionDR->setEnabled(typeObject["DR"].toBool());
                ui->action_Assistant_GNSS_Offline->setEnabled(typeObject["AGNSS"].toBool());
                ui->action_Assistant_GNSS_Online->setEnabled(typeObject["AGNSS_OL"].toBool());
                ui->actionConfigureration_View->setEnabled(typeObject["configuration"].toBool());
                ui->action_StaticTtffTesting->setEnabled(typeObject["TTFF"].toBool());
                //                            m_ui->actionColdStart->setEnabled(typeObject["TTFF"].toBool());
                //                            m_ui->actionWarmStart->setEnabled(typeObject["TTFF"].toBool());
                //                            m_ui->actionHotStart->setEnabled(typeObject["TTFF"].toBool());
                ui->actionNTRIP_Client->setEnabled(typeObject["NTRIP"].toBool());
                if (typeObject["isNMEA_CMD"].toBool())
                {
                    ttff_cmd_info[Cold] = typeObject["Cold"].toString().toLatin1();
                    ttff_cmd_info[Warm] = typeObject["Warm"].toString().toLatin1();
                    ttff_cmd_info[Hot] = typeObject["Hot"].toString().toLatin1();
                    ttff_cmd_info[FullCold] = typeObject["Full"].toString().toLatin1();
                }
                else
                {
                    ttff_cmd_info[Cold] = QByteArray::fromHex(typeObject["Cold"].toString().toLocal8Bit());
                    ttff_cmd_info[Warm] = QByteArray::fromHex(typeObject["Warm"].toString().toLocal8Bit());
                    ttff_cmd_info[Hot] = QByteArray::fromHex(typeObject["Hot"].toString().toLocal8Bit());
                    ttff_cmd_info[FullCold] = QByteArray::fromHex(typeObject["Full"].toString().toLocal8Bit());
                }
                ui->actionColdStart->setEnabled(!ttff_cmd_info[Cold].isEmpty());
                ui->actionWarmStart->setEnabled(!ttff_cmd_info[Warm].isEmpty());
                ui->actionHotStart->setEnabled(!ttff_cmd_info[Hot].isEmpty());
                ui->actionFullColdStart->setEnabled(!ttff_cmd_info[FullCold].isEmpty());
                return;
                //                        }
                //                    }
                //                }
            }
        }
    }
}

void MainWindow::OpenQGNSS_Log()
{
    QProcess::startDetached("./QGNSSLog.exe", QStringList());
}

void MainWindow::updateFirmware()
{
    auto AppMange_ = QGNSS_FLASHTOOL::get_instance();
    if (!AppMange_->flashtool_get_io_instance(m_serial))
    {
        return;
    }
    QString md_name = dev_info.stringModelType;
    BROM_BASE::QGNSS_Model md = (BROM_BASE::QGNSS_Model)IODvice::Enum_Key2Value<BROM_BASE::QGNSS_Model>(md_name.trimmed());
    AppMange_->flashtool_show(md, this);
}

void MainWindow::turn_Cmd_console_onoff()
{
    static commandDialog *m_commanddialog = nullptr;
    if (m_commanddialog == nullptr)
    {
        m_commanddialog = new commandDialog(this);
        connect(m_commanddialog, SIGNAL(sendData(const char *, unsigned int)), this, SLOT(sendData(const char *, unsigned int)));
        connect(m_commanddialog, SIGNAL(sendData(QByteArray)), this, SLOT(sendData(QByteArray)));
        m_commanddialog->show();
    }
    else
        m_commanddialog->show();
}

void MainWindow::turn_Configuration_view_onoff()
{
    //    QMetaEnum curType = QMetaEnum::fromType<QGNSS_Model>();
    //    int type = curType.keyToValue(m_settings->m_module_name.toLocal8Bit());
    switch (dev_info.model_type)
    {
    case quectel_gnss::LC26G:
    case quectel_gnss::LC76G:
    case quectel_gnss::LC86G:
    {
        auto *pLC26G = new Config_LC26G(this, dev_info.model_type);
        connect(pLC26G, SIGNAL(sendCMD(QByteArray)), this, SLOT(sendData(QByteArray)));
        pLC26G->show();
        break;
    }
    case quectel_gnss::LC29DA:
    case quectel_gnss::LC29DC:
        configurationLC29D->show();
        break;
    case quectel_gnss::LC98S:
    case quectel_gnss::L26T:
    case quectel_gnss::L26P:
    case quectel_gnss::L26UDR:
    case quectel_gnss::L26ADRC:
    case quectel_gnss::L26ADR:
    case quectel_gnss::L26DRAA:
    {
        configuration_L26DR *cL26DR = new configuration_L26DR(this);
        cL26DR->setReceiver_type(dev_info.model_type);
        connect(cL26DR, SIGNAL(sendData(const char *, unsigned int)), this, SLOT(sendData(const char *, unsigned int)));
        connect(m_NMEA_Parser, SIGNAL(parseData_L26DR(QByteArray)), cL26DR, SLOT(updateUI(QByteArray)));
        cL26DR->show();
    }
    break;
    case quectel_gnss::LC79DA:
    {
        configuration_LC79DA *cLC79DA = new configuration_LC79DA(this);
        connect(cLC79DA, SIGNAL(sendData(const char *, unsigned int)), this, SLOT(sendData(const char *, unsigned int)));
        connect(m_NMEA_Parser, SIGNAL(parseData_LC79DA(NMEA_t *)), cLC79DA, SLOT(updateUI(NMEA_t *)));
        cLC79DA->show();
    }
    break;
    case QGNSS_Model::LC79DC:
    {
        configuration_LC79DC *cLC79DC = new configuration_LC79DC(this);
        connect(cLC79DC, SIGNAL(sendData(const char *, unsigned int)), this, SLOT(sendData(const char *, unsigned int)));
        connect(m_NMEA_Parser, SIGNAL(parseData_LC79DC(NMEA_t *)), cLC79DC, SLOT(updateUI(NMEA_t *)));
        cLC79DC->show();
    }
    break;
    case QGNSS_Model::LG69TAS:
    {
        configuration_LG69TAS *cLG69TAS = new configuration_LG69TAS(this);
        connect(cLG69TAS, SIGNAL(sendData(const char *, unsigned int)), this, SLOT(sendData(const char *, unsigned int)));
        connect(m_NMEA_Parser, SIGNAL(parseData_LG69TAS(NMEA_t *)), cLG69TAS, SLOT(updateUI(NMEA_t *)));
        cLG69TAS->show();
    }
    break;
    case QGNSS_Model::LG69TAP:
    {
        configuration_LG69TAP *cLG69TAP = new configuration_LG69TAP(this);
        connect(cLG69TAP, SIGNAL(sendData(const char *, unsigned int)), this, SLOT(sendData(const char *, unsigned int)));
        connect(m_NMEA_Parser, SIGNAL(parseData_LG69TAP(NMEA_t *)), cLG69TAP, SLOT(updateUI(NMEA_t *)));
        cLG69TAP->show();
    }
    break;
    case QGNSS_Model::L76KA:
    {
        configuration_L76K *cL76K = new configuration_L76K();
        cL76K->show();
        break;
    }
    case quectel_gnss::LC29YIA:
    {
        configuration_LC29Y *cLC29Y = new configuration_LC29Y(this);
        cLC29Y->show();
        break;
    }
    case quectel_gnss::LG69TAA:
    {
        configuration_LG69TAA *configDialog = new configuration_LG69TAA();
        configDialog->show();
        break;
    }
    case quectel_gnss::LG69TAM:
    case quectel_gnss::LG69TAQ:
    {
        auto *configDialog = new configuration_P1(this);
        configDialog->setModel_type(dev_info.model_type);
        connect(configDialog, SIGNAL(sendData(QByteArray)), this, SLOT(sendData(QByteArray)));
        connect(this, SIGNAL(parseData(QByteArray)), configDialog, SLOT(updateUI(QByteArray)));
        configDialog->show();
        break;
    }
    default:
        qDebug() << dev_info.stringModelType << "is not supported.";
        break;
    }
}

void MainWindow::turn_Static_TTFF_testing_Dialog_onoff()
{
    TTFF_TestDialog *TTFF_dialog = new TTFF_TestDialog(this);
    connect(TTFF_dialog, &TTFF_TestDialog::openSetAGNSS_Dialog, this, &MainWindow::on_action_Assistant_GNSS_Online_triggered);
    connect(TTFF_dialog, &TTFF_TestDialog::sendAgnssData, m_FtpClientDialog, &FtpClientDialog::sendAidingDataToModule);
    TTFF_dialog->setSolution_data(&NMEA_Data.Solution_Data);
    TTFF_dialog->show();
}

void MainWindow::turn_DR_Information_Dialog_onoff()
{
    //"L26-DR"));//3
    DR_Dialog *dr_dialog = new DR_Dialog(this);
    dr_dialog->setNMEA_Data_t(&NMEA_Data);
    dr_dialog->show();
}

void MainWindow::ShowLogFilesInExplorer()
{
    QDesktopServices::openUrl(QUrl::fromLocalFile("./logFile"));
}

void MainWindow::sendResetCMD(int type)
{
    QByteArray cmd;
    if (type < 5)
        cmd = ttff_cmd_info[type];
    if (m_serial->isOpen())
    {
        clickDTR();
        clickRTS();
        //------------------
        sendData(cmd.data(), static_cast<uint32_t>(cmd.length()));
        qgnss_sleep(580);
        clearAllData();
        NMEA_Data.lastTTFF = nullopt;
    }
}

void MainWindow::send_GNSS_cold_start()
{
    sendResetCMD(RestartType::Cold);
}
void MainWindow::send_GNSS_warm_start()
{
    sendResetCMD(RestartType::Warm);
}
void MainWindow::send_GNSS_hot_start()
{
    sendResetCMD(RestartType::Hot);
}

int MainWindow::sendAndWaitForACK(QByteArray cmd, const unsigned int &nACK)
{
    uint32_t curN;
    QTime timer;
    for (int i = 0; i < 3; i++)
    {
        curN = nACK;
        sendData(cmd);
        timer.restart();
        while (nACK == curN)
        {
            QCoreApplication::processEvents();
            if (timer.elapsed() > 600)
                break;
        }
        if (nACK > curN)
            return 1;
    }
    qDebug() << "No reply:" << cmd;
    return 0;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    m_serial->close();
    writeSettings();
    ui->mdiArea->closeAllSubWindows();
    //    if (ui->mdiArea->currentSubWindow())
    //    {
    //        event->ignore();
    //    }
    //    else
    //    {
    event->accept();
    //    }
}

void MainWindow::sendData(const char *data, unsigned int size)
{
    if (m_serial->isOpen())
    {
        clickDTR();
        clickRTS();
        //------------------
        m_serial->write(data, size);
        qgnss_sleep(20);
        //m_serial->waitForBytesWritten();
        QByteArray info(data, size);
        QByteArray w = QString("W->\r\n   HEX: %2\r\nString: %1\r\n").arg(info.data()).arg(info.toHex(' ').data()).toLocal8Bit();
        emit displayTextData(w);
        qInfo() << "Send data:" << info;
    }
}

void MainWindow::sendData(const QByteArray &data)
{
    if (m_serial->isOpen())
    {
        clickDTR();
        clickRTS();
        //------------------
        m_serial->write(data);
        //        m_serial->waitForBytesWritten();
        QByteArray w = QString("W->\r\n   HEX: %2\r\nString: %1\r\n").arg(data.data()).arg(data.toHex(' ').data()).toLocal8Bit();
        emit displayTextData(w);
        qInfo() << "Write data:" << data;
    }
}

void MainWindow::update_online_map_view(bool judge)
{
    QString cmd, lon, lat;
    // qDebug() << "NMEA_Data.Solution_Data.useSetence:" << NMEA_Data.Solution_Data.useSetence;
    if (judge)
    {
        if (NMEA_Data.Solution_Data.savedData_L.size())
        {
            // The update, only if it is different from the previous point
            if (fabs(oMapCurlon - NMEA_Data.Solution_Data.savedData_L.last().Longitude) < 0.00005 && fabs(oMapCurlat - NMEA_Data.Solution_Data.savedData_L.last().Latitude) < 0.00005)
            {
                return;
            }
            oMapCurlon = NMEA_Data.Solution_Data.savedData_L.last().Longitude;
            oMapCurlat = NMEA_Data.Solution_Data.savedData_L.last().Latitude;
            lon = QString::number(oMapCurlon, 'f', 8);
            lat = QString::number(oMapCurlat, 'f', 8);
        }
        else
        {
            return;
        }
    }
    cmd = "addmarker(" + lon + "," + lat + ")";
    m_mapWebWidget->addMark(cmd);
}

void MainWindow::replayFromFile()
{
    if (m_playerControl->getLogFileName())
    {
        m_playerControl->show();
        ui->actionConnect->setEnabled(false);
        ui->actionReplayFile->setEnabled(false);
        clearAllData();
        createRTCM2NMEA_LogFile();
    }
}

void MainWindow::ReadDataFromFile(QByteArray data)
{
    emit displayDumpData(data);
    Analyze(data);
}

void MainWindow::ReadDataFromSerial()
{
    if (QGNSS_FLASHTOOL::get_instance()->isopen())
    {
        return;
    }

    QByteArray data;
    data = m_serial->readAll();
    emit displayDumpData(data);
    if (m_storeStatus)
    {
        //        if (m_storeFile->open(QFile::Append))
        //        {

        //            m_storeFile->write(data.data(), data.size());
        //            m_storeFile->close();
        //        }
        auto spdlog_ = spdlog::get(spdlogname.toStdString());
        spdlog_->info(data.toStdString());
    }
    // soft FlowControl specialization --start
//    if (dev_info.softFlowControl)
//    {
//        bool mark = false;
//        for (int i = 0; i < data.size(); ++i)
//        {
//            if (static_cast<uint8_t>(data.at(i)) == 0x77)
//            {
//                mark = true;
//                continue;
//            }
//            if (mark)
//            {
//                mark = false;
//                if (static_cast<uint8_t>(data.at(i)) == 0x88)
//                {
//                    data[i - 1] = 0x77;
//                    data.remove(i, 1);
//                }
//                else if (static_cast<uint8_t>(data.at(i)) == 0xEE)
//                {
//                    data[i - 1] = 0x11;
//                    data.remove(i, 1);
//                }
//                else if (static_cast<uint8_t>(data.at(i)) == 0xEC)
//                {
//                    data[i - 1] = 0x13;
//                    data.remove(i, 1);
//                }
//            }
//        }
//    }
    // soft FlowControl specialization --end
    Analyze(data);
}

void MainWindow::Analyze(QByteArray &data)
{
    QByteArray RTCM2NMEA_str;
    rawDataParser->inputData(data);
    while (!g_rtcm3_queue.isEmpty())
    {
        emit displayHexData(g_rtcm3_queue.takeFirst());
    }
    RTCM2NMEA_str = RTCM2NMEA(rawDataParser->RTCM_Stes_Q);
    if (RTCM2NMEA_str != "None")
    {
        // qDebug() << RTCM2NMEA_str;
        QList<QByteArray> NMEA_List = RTCM2NMEA_str.split(' ');
        // The last element is empty, so remove
        NMEA_List.takeLast();
        rawDataParser->NMEA_Stes_Q.clear();
        rawDataParser->NMEA_Stes_Q.append(NMEA_List);
    }
    m_NMEA_Parser->parseNMEA(rawDataParser->NMEA_Stes_Q);
    m_mtk_bin_parser->parseMTK_Bin(rawDataParser->binaryMTK_Stes_Q);
    configurationLC29D->parseBreamMsg(rawDataParser->breamStes_Q);
    CASIC_Parser::getInstance()->parseCASIC_Msg(rawDataParser->CASIC_Stes_Q);
    while (!rawDataParser->P1_Stes_Q.isEmpty())
    {
        emit parseData(rawDataParser->P1_Stes_Q.takeFirst());
    }
}

void MainWindow::delayTask()
{
    // check update
    QCoreApplication::setApplicationVersion(QString(App_mainVersion) + "." + App_subVersion);
    QSimpleUpdater::getInstance()->checkForUpdates(Last_VersionURL);

#ifndef OPEN_ONLINEMAP
    turn_Position_window_onoff();
#endif
}

void MainWindow::clickDTR()
{
    if(!m_serial->isOpen()) return;
    else if(DTR->isChecked()){
        m_serial->setDataTerminalReady(true);
    }else{
        m_serial->setDataTerminalReady(false);
    }
}

void MainWindow::clickRTS()
{
    if(!m_serial->isOpen()) return;
    else if(RTS->isChecked()){
        m_serial->setRequestToSend(true);
    }else{
        m_serial->setRequestToSend(false);
    }
}

bool MainWindow::createSpdlog()
{
    try
    {
        auto spdlog_ = spdlog::get(spdlogname.toStdString());
        if (spdlog_)
        {
            spdlog_->flush();
            spdlog::drop(spdlogname.toStdString());
        }
        QString perfore_file_name;
        QDateTime time = QDateTime::currentDateTime();
        /* save UTC time */
        if (dev_info.stringModelType.length() > 0)
        {
            perfore_file_name = dev_info.stringModelType + "-" + time.toString("MMdd_hhmmss_") + portName;
        }
        else
        {
            perfore_file_name = "CommonModule_" + time.toString("MMdd_hhmmss_") + portName;
        }
        perfore_file_name = "./logFile/" + perfore_file_name + ".log";
        QSettings settings(SETTINGS_FILE_NAME, QSettings::IniFormat);
        std::shared_ptr<spdlog::logger> loger;
        if(settings.value(LogType_Key, true).toBool()){
            QString v = settings.value(MAX_Log_Key, "2 GB").toString();
            uint32_t size = v.split(' ').at(0).toUInt();
            if(size < 100){
                size *= 1024;
            }
            loger = spdlog::rotating_logger_st<spdlog::async_factory>(spdlogname.toStdString(), perfore_file_name.toStdString(), size*1024*1024, 100);
        }else{
            auto t = QDateTime::fromString("01:00", "hh:mm");
            auto v = settings.value(DailyTime_Key, t).toDateTime();
            int hour = v.time().hour();
            int minute = v.time().minute();
            loger = spdlog::daily_logger_st<spdlog::async_factory>(spdlogname.toStdString(), perfore_file_name.toStdString(), hour, minute);
        }
        auto formatter = std::make_unique<spdlog::pattern_formatter>("%v", spdlog::pattern_time_type::local, "");
        loger->set_formatter(std::move(formatter));
        return true;
    }
    catch (spdlog::spdlog_ex &ex)
    {
        qInfo() << "createSpdlog error " << ex.what();
        return false;
    }
    return true;
}

void MainWindow::handleError(QSerialPort::SerialPortError error)
{
    if (error == QSerialPort::ResourceError)
    {
        QMessageBox::critical(this, tr("Critical Error"), "The serial port is disconnected.");
        ui->actionConnect->setChecked(false);
    }
}

void MainWindow::initActionsConnections()
{
    connect(ui->actionSaveReceivedDataToFile, &QAction::triggered, this, &MainWindow::ShowLogFilesInExplorer);
    connect(ui->actionQuit, &QAction::triggered, this, &MainWindow::close);
    connect(ui->actionClear, &QAction::triggered, this, &MainWindow::clearUI);
    connect(ui->actionFirmware_Download, &QAction::triggered, this, &MainWindow::updateFirmware);
    connect(ui->actionQlog, &QAction::triggered, this, &MainWindow::OpenQGNSS_Log);
    connect(ui->actionCmd_console, &QAction::triggered, this, &MainWindow::turn_Cmd_console_onoff);
    connect(ui->actionConfigureration_View, &QAction::triggered, this, &MainWindow::turn_Configuration_view_onoff);
    connect(ui->action_StaticTtffTesting, &QAction::triggered, this, &MainWindow::turn_Static_TTFF_testing_Dialog_onoff);
    connect(ui->actionPosition, &QAction::triggered, this, &MainWindow::turn_Position_window_onoff);
    connect(ui->actionSky_Map, &QAction::triggered, this, &MainWindow::turn_Sky_Map_window_onoff);
    connect(ui->actionColdStart, SIGNAL(triggered()), this, SLOT(send_GNSS_cold_start()));
    connect(ui->actionWarmStart, &QAction::triggered, this, &MainWindow::send_GNSS_warm_start);
    connect(ui->actionHotStart, &QAction::triggered, this, &MainWindow::send_GNSS_hot_start);
    // file replay
    connect(ui->actionReplayFile, &QAction::triggered, this, &MainWindow::replayFromFile);
    //    connect(&m_replayFile_Timer1, SIGNAL(timeout()), this, SLOT(replayFile_Timer1Timer()));
    /* DR  */
    connect(ui->actionDR, &QAction::triggered, this, &MainWindow::turn_DR_Information_Dialog_onoff);

    connect(m_serial, &QSerialPort::errorOccurred, this, &MainWindow::handleError);
    connect(m_serial, &QSerialPort::readyRead, this, &MainWindow::ReadDataFromSerial);
    connect(m_FtpClientDialog, SIGNAL(sendData(const char *, unsigned int)), this, SLOT(sendData(const char *, unsigned int)));
}

void MainWindow::showStatusMessage(const QString &message)
{
    m_status->setText(message);
    qInfo() << message;
}

void MainWindow::setSubwindowDefaultLayout()
{
    if (ui->mdiArea->width() > 1600)
    {
        int mdiWidth;
        mdiWidth = ui->mdiArea->width();
        m_signallevelWidget->parentWidget()->move(0, 0);
        m_signallevelWidget->parentWidget()->resize(mdiWidth / 3 * 2, ui->mdiArea->height() / 2);
        m_skyViewWidget->parentWidget()->move(mdiWidth / 3 * 2, 0);
        m_skyViewWidget->parentWidget()->resize(mdiWidth / 3, ui->mdiArea->height() / 2);
#ifdef OPEN_ONLINEMAP
        m_mapWebWidget->parentWidget()->move(mdiWidth / 3 * 2, ui->mdiArea->height() / 2);
        m_mapWebWidget->parentWidget()->resize(mdiWidth / 3, ui->mdiArea->height() / 2);
#endif
        foreach (auto var, ui->mdiArea->subWindowList())
        {
            if (var->windowTitle() == D_TEXT_WINDOW_NAME)
            {
                var->resize(mdiWidth / 3, ui->mdiArea->height() / 2);
                var->move(mdiWidth / 3, ui->mdiArea->height() / 2);
            }
            else if (var->windowTitle() == D_TEXT_WINDOW_NAME)
            {
                var->resize(mdiWidth / 3, ui->mdiArea->height() / 2);
                var->move(mdiWidth / 3, ui->mdiArea->height() / 2);
            }
#ifndef OPEN_ONLINEMAP
            else if (var->windowTitle() == D_DEVIATION_WINDOW_NAME)
            {
                var->resize(mdiWidth / 3, m_ui->mdiArea->height() / 2);
                var->move(mdiWidth / 3 * 2, m_ui->mdiArea->height() / 2);
            }
#endif
        }
    }
    else
    {
        //        m_ui->mdiArea->cascadeSubWindows();
    }
}

void MainWindow::on_actionTianDi_map_triggered()
{
    ui->actionOSM_map->setChecked(false);
    m_mapWebWidget->switchOnlineMap(MapWebViewWindow::TianDiMap);
}

void MainWindow::on_actionOSM_map_triggered()
{
    ui->actionTianDi_map->setChecked(false);
    m_mapWebWidget->switchOnlineMap(MapWebViewWindow::OSM);
}

bool MainWindow::enableLogging()
{
    QString perfore_file_name;
    QDateTime time = QDateTime::currentDateTime();
    /* save UTC time */
    if (dev_info.stringModelType.length() > 0)
    {
        perfore_file_name = dev_info.stringModelType + "-" + time.toString("MMdd_hhmmss_") + portName;
    }
    else
    {
        perfore_file_name = "CommonModule_" + time.toString("MMdd_hhmmss_") + portName;
    }
    if (m_storeFile != nullptr)
    {
        m_storeFile->close();
    }
    m_storeFile = new QFile("./logFile/" + perfore_file_name + ".log");
    if (m_storeFile->open(QFile::ReadWrite | QFile::Append))
    {
        m_storeFile->close();
        return true;
    }
    return false;
}

void MainWindow::on_actionConnect_toggled(bool checked)
{
    if (checked)
    {
        portName = this->dev_info.portName;
        m_serial->setPortName(dev_info.portName);
        m_serial->setBaudRate(dev_info.baudRate);
        m_serial->setDataBits(dev_info.dataBits);
        m_serial->setParity(dev_info.parity);
        m_serial->setStopBits(dev_info.stopBits);
        m_serial->setFlowControl(dev_info.flowControl);
        if (m_serial->open(QIODevice::ReadWrite))
        {
            ui->actionDeviceInfo->setEnabled(false);
            showStatusMessage(tr("Connected to %1 : %2 | Module: %3")
                                  .arg(dev_info.portName)
                                  .arg(dev_info.stringBaudRate)
                                  .arg(dev_info.stringModelType));
            //            stream_led->setLight(true);
            ui->actionReplayFile->setEnabled(false);
            // Opens the functions supported by the selected module
            loadModelConfig();
            createRTCM2NMEA_LogFile(dev_info.portName);
            clearAllData();
            //            QMetaEnum curType = QMetaEnum::fromType<QGNSS_Model>();
            modelIndex = dev_info.model_type;
            // m_storeStatus = enableLogging();
            m_storeStatus = createSpdlog();
        }
        else
        {
            QMessageBox::critical(this, tr("Error"), "Serial opening failed.");
            ui->actionConnect->setChecked(false);
            showStatusMessage(tr("Open error"));
        }
    }
    else
    {
        m_serial->close();
        // Turn off other functions
        ui->actionDR->setEnabled(false);
        ui->action_StaticTtffTesting->setEnabled(false);
        ui->action_Assistant_GNSS_Offline->setEnabled(false);
        ui->action_Assistant_GNSS_Online->setEnabled(false);
        ui->actionConfigureration_View->setEnabled(false);
        ui->actionColdStart->setEnabled(false);
        ui->actionWarmStart->setEnabled(false);
        ui->actionHotStart->setEnabled(false);
        ui->actionFullColdStart->setEnabled(false);
        ui->actionDeviceInfo->setEnabled(true);
        ui->actionReplayFile->setEnabled(true);
        showStatusMessage(tr("Disconnected"));
    }
}

void MainWindow::on_actionAbout_triggered()
{
    About *about = new About(this);
    about->show();
}

void MainWindow::updateDataView()
{
    QString tempStr;
    // Date Time
    if (NMEA_Data.Solution_Data.Hour.has_value())
    {
        ui->time_LineEdit->setText(QString().sprintf("%02d:%02d:%06.3f",
                                                     NMEA_Data.Solution_Data.Hour.value(),
                                                     NMEA_Data.Solution_Data.Minute.value(),
                                                     NMEA_Data.Solution_Data.dSecond.value()));
    }
    if (NMEA_Data.Solution_Data.Year.has_value())
    {
        ui->date_LineEdit->setText(QString().sprintf("%04d-%02d-%02d",
                                                     NMEA_Data.Solution_Data.Year.value(),
                                                     NMEA_Data.Solution_Data.Month.value(),
                                                     NMEA_Data.Solution_Data.Day.value()));
    }
    if (NMEA_Data.Solution_Data.savedData_L.size())
    {
        tempStr = QString::number(NMEA_Data.Solution_Data.savedData_L.last().Longitude, 'f', 8);
        ui->longitude_LineEdit->setText(tempStr);
        tempStr = QString::number(NMEA_Data.Solution_Data.savedData_L.last().Latitude, 'f', 8);
        ui->latitude_LineEdit->setText(tempStr);
        tempStr = QString::number(NMEA_Data.Solution_Data.savedData_L.last().AltitudeMSL, 'f', 2);
        ui->altitude_LineEdit->setText(tempStr);

        if(strlen(NMEA_Data.GGA_Data.GeoidalSep)){
            double GS = stod(NMEA_Data.GGA_Data.GeoidalSep);
            tempStr = QString::number(NMEA_Data.Solution_Data.savedData_L.last().AltitudeMSL + GS, 'f', 2);
            ui->lineEdit_EPH->setText(tempStr);
        }
    }
    // SpeedKnots
    if (NMEA_Data.Solution_Data.SpeedKnots.has_value())
    {
        tempStr = QString::number(NMEA_Data.RMC_Data.m_dSpeedKnots * KT2KMH, 'f', 2);
        ui->speed_LineEdit->setText(tempStr);
    }
    // FixMode
    if (NMEA_Data.Solution_Data.FixMode.has_value())
    {
        ui->lineEdit_fixMode->setText(FixMode_m[NMEA_Data.Solution_Data.FixMode.value()]);
    }
    // PDOP
    if (NMEA_Data.Solution_Data.PDOP.has_value())
    {
        tempStr = QString::number(NMEA_Data.Solution_Data.PDOP.value(), 'f', 2);
        ui->pDOP_LineEdit->setText(tempStr);
    }
    // HDOP
    if (NMEA_Data.Solution_Data.HDOP.has_value())
    {
        tempStr = QString::number(NMEA_Data.Solution_Data.HDOP.value(), 'f', 2);
        ui->hDOP_LineEdit->setText(tempStr);
    }
    // AgeOfDiff
    if (NMEA_Data.Solution_Data.AgeOfDiff.has_value())
    {
        tempStr = QString::number(NMEA_Data.Solution_Data.AgeOfDiff.value(), 'f', 1);
        ui->LineEdit_AgeOfDiff->setText(tempStr);
    }
    // GGA
    if (NMEA_Data.GGA_Data._n)
    {
        tempStr = QString::number(NMEA_Data.GGA_Data._n);
        ui->totalTimes_LineEdit->setText(tempStr);
        tempStr = QString::number(NMEA_Data.GGA_Data._Fixed);
        ui->fixedPoints_LineEdit->setText(tempStr);
        tempStr = QString::number(NMEA_Data.GGA_Data._RTK);
        ui->rtkFixedSoution_LineEdit->setText(tempStr);
        tempStr = QString::number(NMEA_Data.GGA_Data._FloatRTK);
        ui->rtkFloatSoution_LineEdit->setText(tempStr);
        ui->QualityIndicator_LineEdit->setText(QualityIndicator_m[NMEA_Data.GGA_Data.m_nGPSQuality]);
    }
    if (NMEA_Data.lastTTFF.has_value())
    {
        tempStr = QString::number(NMEA_Data.lastTTFF.value() / 1000.0, 'f', 3);
        ui->lastTTFF_LineEdit->setText((NMEA_Data.lastTTFF < 500) ? "" : tempStr);
    }
    if (NMEA_Data.Solution_Data.ACC_2D.has_value())
    {
        tempStr = QString::number(NMEA_Data.Solution_Data.ACC_2D.value(), 'f', 4);
        ui->LineEdit_2d_acc->setText(tempStr);
    }
    if (NMEA_Data.Solution_Data.ACC_3D.has_value())
    {
        tempStr = QString::number(NMEA_Data.Solution_Data.ACC_3D.value(), 'f', 4);
        ui->LineEdit_3d_acc->setText(tempStr);
    }
    if (NMEA_Data.Solution_Data.DR_Type.has_value())
    {
        ui->lineEdit_DR_Type->setHidden(false);
        ui->label_DR_Type->setHidden(false);
        ui->lineEdit_DR_Type->setText(DR_Type[NMEA_Data.Solution_Data.DR_Type.value()]);
    }
    else{
        ui->lineEdit_DR_Type->setHidden(true);
        ui->label_DR_Type->setHidden(true);
    }
}

void MainWindow::clearAllData()
{
    if (m_serial->isOpen())
    {
        clickDTR();
        clickRTS();
        //------------------
        m_serial->clear(QSerialPort::Input);
        m_serial->readAll();
        qDebug() << "serial->clear";
    }
    rawDataParser->mainMsg = Unknown;
    rawDataParser->clearData();
    qDebug() << "rawDataParser clear";
    m_NMEA_Parser->ResetNMEA_Data();
    qDebug() << "m_NMEA_Parser clear";
    initRTCM3Parser();
    qDebug() << "initRTCM3Parser";
    clearUI();
    qDebug() << "clearUI";
}

void MainWindow::clearUI()
{
    oMapCurlon = 0;
    oMapCurlat = 0;
    m_skyViewWidget->update_skymap_view();
    ui->lastTTFF_LineEdit->clear();
    ui->LineEdit_2d_acc->clear();
    ui->LineEdit_3d_acc->clear();
    ui->longitude_LineEdit->clear();
    ui->latitude_LineEdit->clear();
    ui->altitude_LineEdit->clear();
    ui->speed_LineEdit->clear();
    ui->hDOP_LineEdit->clear();
    ui->pDOP_LineEdit->clear();
    ui->time_LineEdit->clear();
    ui->date_LineEdit->clear();
    ui->totalTimes_LineEdit->clear();
    ui->fixedPoints_LineEdit->clear();
    ui->rtkFloatSoution_LineEdit->clear();
    ui->rtkFixedSoution_LineEdit->clear();
    ui->lineEdit_fixMode->clear();
    ui->QualityIndicator_LineEdit->clear();
    ui->LineEdit_AgeOfDiff->clear();
#ifdef OPEN_ONLINEMAP
    m_mapWebWidget->clearOverlays();
#endif
    emit clearData();
}

void MainWindow::updateUI()
{
    //    if (NMEA_Data._n > refreshUI_Flag)
    //    {
    // Data View
    updateDataView();
    // Sky View
    m_skyViewWidget->update_skymap_view();
    // Signel View
    m_signallevelWidget->updateGNSSView(NMEA_Data.Solution_Data);
//        m_signallevelWidget->updateGNSSView(NMEA_Data.GSV_Data);
// OnlineMap
#ifdef OPEN_ONLINEMAP
    update_online_map_view();
#endif

    //        refreshUI_Flag = NMEA_Data._n;
    //    }
}

void MainWindow::on_actionNTRIP_Client_triggered()
{
    if (m_ntripclientdialog == nullptr)
    {
        m_ntripclientdialog = new NTRIP_ClientDialog(this);
        m_ntripclientdialog->setGgaStr(NMEA_Data.GGA_Data.GGA_String);
        connect(m_ntripclientdialog, SIGNAL(send2Model(QByteArray)), this, SLOT(sendData(QByteArray)));
        m_ntripclientdialog->show();
    }
    else
    {
        m_ntripclientdialog->show();
    }
}

void MainWindow::on_actionCheck_for_updates_triggered()
{
    QCoreApplication::setApplicationVersion(QString(App_mainVersion) + "." + App_subVersion);
    QSimpleUpdater::getInstance()->checkForUpdates(Last_VersionURL);
    if (QSimpleUpdater::getInstance()->getUpdateAvailable(Last_VersionURL))
    {
        qDebug() << "on_actionCheck_for_updates_triggered";
    }
    else
    {
        qDebug() << "last version";
        QMessageBox::information(this, "Check for updates", "This is the latest version.", QMessageBox::Ok);
    }
}

void MainWindow::on_actionMessages_View_triggered()
{
    Msg_Dialog *dialog = new Msg_Dialog(this);
    dialog->show();
    connect(m_NMEA_Parser, SIGNAL(parseDataLC02H(NMEA_t)), dialog, SLOT(getParseDataLC02H(NMEA_t )));
}

void MainWindow::on_actionFullColdStart_triggered()
{
    sendResetCMD(RestartType::FullCold);
}

void MainWindow::on_action_Assistant_GNSS_Offline_triggered()
{
    //    QMetaEnum curType = QMetaEnum::fromType<QGNSS_Model>();
    switch (dev_info.model_type)
    {
    case QGNSS_Model::L89HA:
    case QGNSS_Model::LC29HAA:
    case QGNSS_Model::LC26G:
    case QGNSS_Model::LC76G:
    case QGNSS_Model::LC86G:
    {
        AGNSS_LC29H_Dialog *agnss = new AGNSS_LC29H_Dialog(this);
        agnss->show();
        break;
    }
    case QGNSS_Model::LC76F /*A*/:
    {
        AGNSS_LC76F_Dialog *agnss = new AGNSS_LC76F_Dialog(this);
        agnss->show();
        break;
    }
    case QGNSS_Model::LC79DA:
    {
        AGNSS_LC79D_Dialog *agnss = new AGNSS_LC79D_Dialog(this);
        agnss->show();
        break;
    }
    case QGNSS_Model::LC760Z:
    case QGNSS_Model::LC260Z:
    {
        auto *agnss = new HD8120_offline_Dialog(this);
        agnss->show();
        break;
    }
    default: // L76     LC79DA on top
        m_FtpClientDialog->setVisible(!m_FtpClientDialog->isVisible());
        if (m_FtpClientDialog->isVisible())
            m_FtpClientDialog->switch_agnss_online_offline_mode(0);
        break;
    }
}

void MainWindow::on_action_Assistant_GNSS_Online_triggered()
{
    //    QMetaEnum curType = QMetaEnum::fromType<QGNSS_Model>();
    switch (dev_info.model_type)
    {
    case QGNSS_Model::L76KA:
    {
        AGNSS_L76K_Dialog *agnss = new AGNSS_L76K_Dialog(this);
        agnss->show();
        break;
    }
    case QGNSS_Model::L89HA:
    case QGNSS_Model::LC29HAA:
    case QGNSS_Model::LC26G:
    case QGNSS_Model::LC76G:
    case QGNSS_Model::LC86G:
    {
        AGNSS_LC29H_hostDialog *agnss = new AGNSS_LC29H_hostDialog(this);
        agnss->show();
        break;
    }
    case QGNSS_Model::LC29YIA:
    case QGNSS_Model::LC760Z:
    case QGNSS_Model::LC260Z:
    {
        auto *agnss = new AGNSS_LC29Y_Online_Dialog(this);
        agnss->show();
        break;
    }
    default: // L76  LC29D  L26DR
        if (ui->action_Assistant_GNSS_Online->isEnabled())
        {
            m_FtpClientDialog->setVisible(!m_FtpClientDialog->isVisible());
            if (m_FtpClientDialog->isVisible())
            {
                m_FtpClientDialog->switch_agnss_online_offline_mode(1);
            }
        }
        break;
    }
}

// SubWindow
void MainWindow::on_actionRawDataConsole_triggered()
{
    Console *w = new Console();
    connect(this, SIGNAL(clearData()), w, SLOT(on_clearText_clicked()));
    connect(this, SIGNAL(displayDumpData(QByteArray)), w, SLOT(displayDump(QByteArray)));
    QMdiSubWindow *subWindow = ui->mdiArea->addSubWindow(w);
    subWindow->setWindowTitle(D_RAW_WINDOW_NAME);
    subWindow->setWindowIcon(QIcon(":/images/rawConsole.png"));
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    subWindow->resize(554, 447);
    subWindow->show();
}

void MainWindow::on_actionBin_console_triggered()
{
    Console *w = new Console();
    connect(this, SIGNAL(clearData()), w, SLOT(on_clearText_clicked()));
    connect(this, SIGNAL(displayHexData(QByteArray)), w, SLOT(displayHex(QByteArray)));
    connect(rawDataParser, SIGNAL(displayHexData(QByteArray)), w, SLOT(displayHex(QByteArray)));
    QMdiSubWindow *subWindow = ui->mdiArea->addSubWindow(w);
    subWindow->setWindowTitle(D_BINARY_WINDOW_NAME);
    subWindow->setWindowIcon(QIcon(":/images/binConsole.png"));
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    subWindow->show();
}

void MainWindow::on_actionTextConsole_triggered()
{
    Console *w = new Console();
    connect(this, SIGNAL(clearData()), w, SLOT(on_clearText_clicked()));
    connect(rawDataParser, SIGNAL(displayTextData(QByteArray)), w, SLOT(displayText(QByteArray)));
    connect(this, SIGNAL(displayTextData(QByteArray)), w, SLOT(displayText(QByteArray)));
    QMdiSubWindow *subWindow = ui->mdiArea->addSubWindow(w);
    subWindow->setWindowTitle(D_TEXT_WINDOW_NAME);
    subWindow->setWindowIcon(QIcon(":/images/nmeaConsole.png"));
    subWindow->setAttribute(Qt::WA_DeleteOnClose);
    subWindow->show();
}
void MainWindow::turn_Sky_Map_window_onoff()
{
    m_skyViewWidget->hide();
    m_skyViewWidget->show();
}

void MainWindow::turn_Position_window_onoff()
{
    m_deviationMapWidget = new Deviation_Map;
    connect(this, SIGNAL(clearData()), m_deviationMapWidget, SLOT(clearMap()));
    m_deviationMapWindow = ui->mdiArea->addSubWindow(m_deviationMapWidget);
    m_deviationMapWindow->setAttribute(Qt::WA_DeleteOnClose, true);
    m_deviationMapWindow->resize(380,408);
    m_deviationMapWindow->show();
}

void MainWindow::on_actionGNSS_signal_triggered()
{

    m_signallevelWidget->hide();
    //    m_signallevelWidget->parentWidget()->show();
    m_signallevelWidget->show();
}

void MainWindow::on_actionOnlineMap_triggered()
{
    m_mapWebWidget->hide();
    m_mapWebWidget->show();
}

void MainWindow::on_actionTile_triggered()
{
    QList<QMdiSubWindow *> sList = ui->mdiArea->subWindowList();
    int vNum = std::count_if(sList.begin(),sList.end(),[](QMdiSubWindow *i){return i->isVisible();});
    if (vNum <= 6
//            && ui->mdiArea->size().width() > 1380
//            && ui->mdiArea->size().height() > 730
            )
    {
        ui->mdiArea->tileSubWindows();
    }
    else
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Too many windows are not available to be tiled !"),
                             QMessageBox::Ok,
                             QMessageBox::Ok);
    }
}

void MainWindow::on_actionCascade_triggered()
{
    int pos = 0;
    SignalLevelWindow *t = ui->mdiArea->findChild<SignalLevelWindow *>();
    if(t){
        if(t->parentWidget()->isVisible()){
            t->parentWidget()->move(0,0);
            pos+=28;
        }
    }
    foreach (auto var, ui->mdiArea->subWindowList())
    {
        if(!var->isVisible())
            continue;
        if (var->windowTitle() != D_SIGNAL_WINDOW_NAME){
            ui->mdiArea->setActiveSubWindow(var);
            var->resize(400,380);
            var->move(pos,pos);
            pos+=28;
        }
    }
}

void MainWindow::on_actionClose_All_triggered()
{
    ui->mdiArea->closeAllSubWindows();
}
// SubWindow --end

void MainWindow::on_actionPreferences_triggered()
{
    preferences->exec();
}

void MainWindow::on_actionDeviceInfo_triggered()
{
    device_info dia(this);
    if (dia.exec() == QDialog::Accepted)
    {
        dev_info = dia.getInfo_t();
        ui->actionConnect->setChecked(true);
    }
}

void MainWindow::readSettings()
{
//    QSettings settings(SETTINGS_FILE_NAME, QSettings::IniFormat);
    QSettings settings;
    // Main window
    QByteArray geometry = settings.value("Geometry/Mainwindow", QByteArray()).toByteArray();
    if (geometry.isEmpty())
    {
        const QRect availableGeometry = QApplication::desktop()->availableGeometry(this);
        resize(availableGeometry.width() / 3, availableGeometry.height() / 2);
        move((availableGeometry.width() - width()) / 2,
             (availableGeometry.height() - height()) / 2);
    }
    else
    {
        restoreGeometry(geometry);
        QCoreApplication::processEvents();
    }
    // Signal Level
    geometry = settings.value("Geometry/MDI/Signal Level", QByteArray()).toByteArray();
    if (geometry.isEmpty())
    {

    }
    else
    {
        m_signallevelWidget->parentWidget()->restoreGeometry(geometry);
        geometry = settings.value("Geometry/MDI/Signal Level_y", QByteArray()).toByteArray();
        m_signallevelWidget->parentWidget()->move(m_signallevelWidget->parentWidget()->pos().x(),geometry.toInt());
        m_signallevelWidget->show();
    }
    // Online Map
    geometry = settings.value("Geometry/MDI/Online Map", QByteArray()).toByteArray();
    if (geometry.isEmpty())
    {
    }
    else
    {
        m_mapWebWidget->parentWidget()->restoreGeometry(geometry);
        geometry = settings.value("Geometry/MDI/Online Map_y", QByteArray()).toByteArray();
        m_mapWebWidget->parentWidget()->move(m_mapWebWidget->parentWidget()->pos().x(),geometry.toInt());
        m_mapWebWidget->show();
    }
    // Sky View
    geometry = settings.value("Geometry/MDI/Sky View", QByteArray()).toByteArray();
    if (geometry.isEmpty())
    {
    }
    else
    {
        m_skyViewWidget->parentWidget()->restoreGeometry(geometry);
        geometry = settings.value("Geometry/MDI/Sky View_y", QByteArray()).toByteArray();
        m_skyViewWidget->parentWidget()->move(m_skyViewWidget->parentWidget()->pos().x(),geometry.toInt());
        m_skyViewWidget->show();
    }
    // Text data
    geometry = settings.value("Geometry/MDI/Text data", QByteArray()).toByteArray();
    if (geometry.isEmpty())
    {
    }
    else
    {
        on_actionTextConsole_triggered();
        foreach (auto var, ui->mdiArea->subWindowList())
        {
            if (var->windowTitle() == D_TEXT_WINDOW_NAME){
                var->restoreGeometry(geometry);
                geometry = settings.value("Geometry/MDI/Text data_y", QByteArray()).toByteArray();
                var->move(var->pos().x(),geometry.toInt());
            }
        }
    }
    // Binary data
    geometry = settings.value("Geometry/MDI/Binary data", QByteArray()).toByteArray();
    if (geometry.isEmpty())
    {
    }
    else
    {
        on_actionBin_console_triggered();
        foreach (auto var, ui->mdiArea->subWindowList())
        {
            if (var->windowTitle() == D_BINARY_WINDOW_NAME){
                var->restoreGeometry(geometry);
                geometry = settings.value("Geometry/MDI/Binary data_y", QByteArray()).toByteArray();
                var->move(var->pos().x(),geometry.toInt());
            }

        }
    }
    // Deviation Map
    geometry = settings.value("Geometry/MDI/Deviation Map", QByteArray()).toByteArray();
    if (geometry.isEmpty())
    {
    }
    else
    {
        turn_Position_window_onoff();
        foreach (auto var, ui->mdiArea->subWindowList())
        {
            if (var->windowTitle() == D_DEVIATION_WINDOW_NAME){
                var->restoreGeometry(geometry);
                geometry = settings.value("Geometry/MDI/Deviation Map_y", QByteArray()).toByteArray();
                var->move(var->pos().x(),geometry.toInt());
            }

        }
    }
}

void MainWindow::writeSettings()
{
    QSettings settings;
//    QSettings settings(SETTINGS_FILE_NAME, QSettings::IniFormat);
    settings.remove("Geometry/MDI");
    settings.setValue("Geometry/Mainwindow", saveGeometry());
    foreach (auto var, ui->mdiArea->subWindowList())
    {
        if (var->isVisible())
        {
            qDebug() << "writeSettings" << var->windowTitle();
            settings.setValue("Geometry/MDI/" + var->windowTitle(), var->saveGeometry());
            settings.setValue("Geometry/MDI/" + var->windowTitle()+"_y", var->pos().y());
        }
    }
}

void MainWindow::on_actionConvert_KML_triggered()
{
    convert_KML cKML;
    cKML.exec();
}

void MainWindow::on_actionCoordinate_Converter_triggered()
{
    CoordinateConverterDialog *cc = new CoordinateConverterDialog(this);
    cc->show();
}

void MainWindow::on_actionDock_Tools_triggered()
{
    ui->rightDockWidget->setVisible(!ui->rightDockWidget->isVisible());
}

void MainWindow::on_actionBase_Station_triggered()
{
    static SurveyView *w = nullptr;
    if(w == nullptr)
    {
        w = new SurveyView(this);
        m_NMEA_Parser->surveyUpdate = std::bind(&SurveyView::updateSurveyInfo, w);
        connect(w, SIGNAL(sendData(QByteArray)), this, SLOT(sendData(QByteArray)));
        QMdiSubWindow *subWindow = nullptr;
        subWindow = ui->mdiArea->addSubWindow(w);
        subWindow->setWindowTitle("Basic Station");
        subWindow->setWindowIcon(QIcon(":/images/BaseStation.png"));
        subWindow->setAttribute(Qt::WA_DeleteOnClose, false);
        subWindow->resize(420, 320);
        subWindow->show();
    }
    else {
        w->show();
    }
}

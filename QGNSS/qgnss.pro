QT += widgets serialport
#QT += charts
QT += webenginewidgets

RC_ICONS = Quectel.ico

DEFINES += _RTLDLL NO_STRICT TRACE ENAGLO ENAQZS ENAGAL ENACMP ENAIRN NFREQ=5 NEXOBS=3
DEFINES += WIN32

CONFIG += c++17

win32:DEFINES += _WINSOCKAPI_

LIBS += -lwinmm
LIBS += -lDbgHelp

TARGET = QGNSS
TEMPLATE = app

include ($$PWD/../QGNSS_BROM/QGNSS_BROM.pri)
include ($$PWD/../QGNSS_FlashTool_UI/QGNSS_FlashTool_UI.pri)
include ($$PWD/../QGNSS_FW_UPG/QGNSS_FW_UPG.pri)
include ($$PWD/../QSimpleUpdater/QSimpleUpdater.pri)
#include ($$PWD/QGNSSLog/QGNSSLog.pri)

SOURCES += \
    AGNSS/CommonEPO_AG3335.cpp \
    AGNSS/FtpClientDialog.cpp \
    AGNSS/LC79D_AGNSS_LTO.c \
    AGNSS/agnss_l76k_dialog.cpp \
    AGNSS/agnss_lc29h_dialog.cpp \
    AGNSS/agnss_lc29h_hostdialog.cpp \
    AGNSS/agnss_lc29y_online_dialog.cpp \
    AGNSS/agnss_lc76f_dialog.cpp \
    AGNSS/agnss_lc79d_dialog.cpp \
    AGNSS/ftp_manager.cpp \
    AGNSS/hd8120_offline_dialog.cpp \
    Configuration/config_lc26g.cpp \
    Configuration/configuration_L26DR.cpp \
    Configuration/configuration_LC79DA.cpp \
    Configuration/configuration_LC79DC.cpp \
    Configuration/configuration_LG69TAP.cpp \
    Configuration/configuration_LG69TAS.cpp \
    Configuration/configuration_l76k.cpp \
    Configuration/configuration_lc29d.cpp \
    Configuration/configuration_lc29y.cpp \
    Configuration/configuration_lg69taa.cpp \
    Configuration/configuration_p1.cpp \
    CoordinateConverterDialog.cpp \
    DataParser/casic_parser.cpp \
    DataParser/extended_rtklib.cpp \
    DataParser/mtk_bin_parser.cpp \
    DataParser/nmea_parser.cpp \
    DataParser/rawdataparser.cpp \
    DataParser/rtcm3parser.cpp \
    Ymodem.cpp \
    YmodemFileReceive.cpp \
    YmodemFileTransmit.cpp \
    convert_kml.cpp \
    device_info.cpp \
    lla_xyz_component.cpp \
    map_scale.cpp \
    preferences_dialog.cpp \
    JsonOperate.cpp \
    RTKLib/rtcm.c \
    RTKLib/rtcm2.c \
    RTKLib/rtcm3.c \
    RTKLib/rtcm3e.c \
    RTKLib/rtkcmn.c \
    SkyViewWidget.cpp \
    about.cpp \
    bream_handler.cpp \
    commandcheckbox.cpp \
    commanddialog.cpp \
    common.cpp \
    console.cpp \
    deviation_map.cpp \
    dr_dialog.cpp \
    ftp_client.cpp \
    imageswitch.cpp \
    main.cpp \
    mainwindow.cpp \
    mapwebviewwindow.cpp \
    msg_dialog.cpp \
    ntrip_clientdialog.cpp \
    playercontrol.cpp \
    processwait.cc \
    qcustomplot.cpp \
    qftp.cpp \
    runmachine.cc \
    signalform.cpp \
    signallevelwindow.cpp \
    startupwidget.cpp \
    surveyview.cpp \
    table_of_ntrip_mount_dialog.cpp \
    ttff_testdialog.cpp \
    worldtimeclock.cpp


HEADERS += \
    AGNSS/CommonEPO_AG3335.h \
    AGNSS/FtpClientDialog.h \
    AGNSS/LC79D_AGNSS_LTO.h \
    AGNSS/agnss_l76k_dialog.h \
    AGNSS/agnss_lc29h_dialog.h \
    AGNSS/agnss_lc29h_hostdialog.h \
    AGNSS/agnss_lc29y_online_dialog.h \
    AGNSS/agnss_lc76f_dialog.h \
    AGNSS/agnss_lc79d_dialog.h \
    AGNSS/ftp_manager.h \
    AGNSS/hd8120_offline_dialog.h \
    Configuration/config_lc26g.h \
    Configuration/configuration_L26DR.h \
    Configuration/configuration_LC79DA.h \
    Configuration/configuration_LC79DC.h \
    Configuration/configuration_LG69TAP.h \
    Configuration/configuration_LG69TAS.h \
    Configuration/configuration_l76k.h \
    Configuration/configuration_lc29d.h \
    Configuration/configuration_lc29y.h \
    Configuration/configuration_lg69taa.h \
    Configuration/configuration_p1.h \
    Configuration/messages_p1/configuration.h \
    Configuration/messages_p1/control.h \
    Configuration/messages_p1/defs.h \
    Configuration/messages_p1/measurements.h \
    CoordinateConverterDialog.h \
    DataParser/casic_parser.h \
    DataParser/mtk_bin_parser.h \
    DataParser/nmea_parser.h \
    DataParser/rawdataparser.h \
    DataParser/rtcm3parser.h \
    Ymodem.h \
    YmodemFileReceive.h \
    YmodemFileTransmit.h \
    appname.h \
    convert_kml.h \
    device_info.h \
    allystar_binary_msg.h\
    JsonOperate.h \
    RTKLib/rtklib.h \
    SkyViewWidget.h \
    about.h \
    bream.h \
    bream_handler.h \
    commandcheckbox.h \
    commanddialog.h \
    common.h \
    console.h \
    deviation_map.h \
    dr_dialog.h \
    ftp_client.h \
    imageswitch.h \
    lla_xyz_component.h \
    mainwindow.h \
    map_scale.h \
    mapwebviewwindow.h \
    msg_dialog.h \
    mtk_bin_parser.h \
    nmea_parser.h \
    ntrip_clientdialog.h \
    playercontrol.h \
    preferences_dialog.h \
    processwait.h \
    qcustomplot.h \
    qftp.h \
    rawdataparser.h \
    rtcm3parser.h \
    runmachine.h \
    signalform.h \
    signallevelwindow.h \
    startupwidget.h \
    surveyview.h \
    table_of_ntrip_mount_dialog.h \
    ttff_testdialog.h \
    worldtimeclock.h


FORMS += \
    AGNSS/FtpClientDialog.ui \
    AGNSS/agnss_l76k_dialog.ui \
    AGNSS/agnss_lc29h_dialog.ui \
    AGNSS/agnss_lc29h_hostdialog.ui \
    AGNSS/agnss_lc29y_online_dialog.ui \
    AGNSS/agnss_lc76f_dialog.ui \
    AGNSS/agnss_lc79d_dialog.ui \
    AGNSS/hd8120_offline_dialog.ui \
    Configuration/config_lc26g.ui \
    Configuration/configuration_L26DR.ui \
    Configuration/configuration_LC79DA.ui \
    Configuration/configuration_LC79DC.ui \
    Configuration/configuration_LG69TAP.ui \
    Configuration/configuration_LG69TAS.ui \
    Configuration/configuration_l76k.ui \
    Configuration/configuration_lc29d.ui \
    Configuration/configuration_lc29y.ui \
    Configuration/configuration_lg69taa.ui \
    Configuration/configuration_p1.ui \
    CoordinateConverterDialog.ui \
    convert_kml.ui \
    device_info.ui \
    lla_xyz_component.ui \
    preferences_dialog.ui \
    SkyViewWidget.ui \
    about.ui \
    commandcheckbox.ui \
    commanddialog.ui \
    console.ui \
    deviation_map.ui \
    dr_dialog.ui \
    ftp_client.ui \
    mainwindow.ui \
    mapwebviewwindow.ui \
    msg_dialog.ui \
    ntrip_clientdialog.ui \
    playercontrol.ui \
    signalform.ui \
    signallevelwindow.ui \
    startupwidget.ui \
    surveyview.ui \
    table_of_ntrip_mount_dialog.ui \
    ttff_testdialog.ui

RESOURCES += \
    terminal.qrc

#CONFIG += debug_and_release
CONFIG(debug, debug|release) {
    DESTDIR += $$PWD/../DevRuntimeEnv/
}else{
    DESTDIR += $$PWD/../DevRuntimeEnv/
    DEFINES += OUTPUT_LOG_TO_FILE
    DEFINES += QT_NO_DEBUG_OUTPUT
}


#QT += network
#QT += uitools
#QMAKE_LFLAGS_RELEASE = /INCREMENTAL:NO /DEBUG
#requires(qtConfig(combobox))
#target.path = $$[QT_INSTALL_EXAMPLES]/serialport/terminal
#INSTALLS += target
#envt.files = $$PWD/../RuntimeComponents/*
#envt.path = $$PWD/../Release/
#INSTALLS += envt

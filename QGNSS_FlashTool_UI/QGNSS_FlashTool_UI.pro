QT +=  widgets   printsupport  core serialport

TEMPLATE = app
TARGET=QGNSS_BROM_TOOL

CONFIG += c++11
CONFIG +=lrelease
RC_FILE+= src/Misc/appInfo.rc

DEFINES+= QMLUI

include (QGNSS_FlashTool_UIcfg.pri)
SOURCES += \
    main.cpp \





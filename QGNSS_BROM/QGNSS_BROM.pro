QT += core serialport

include (../Base_CFG.pri)

TEMPLATE = lib
TARGET =QGNSS_BROM
DEFINES += QGNSS_BROM_LIBRARY
CONFIG += c++11



SOURCES += \
    IO/iodvice.cpp   \
    IO/ioserial.cpp  \
    MTK/mtk_base.cpp \
    MTK/mtk_brom.cpp \
    MTK/mtk_brom_3335.cpp \
    brom_base.cpp \
    keyvalue.cpp

HEADERS += \
    IO/iodvice.h   \
    IO/ioserial.h  \
    MTK/mtk_base.h \
    MTK/mtk_brom.h \
    MTK/mtk_brom_3335.h \
    QGNSS_BROM_global.h \
    brom_base.h \
    keyvalue.h

INCLUDEPATH += \
           IO \
           MTK \



unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

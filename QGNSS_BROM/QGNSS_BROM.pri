QT += core serialport
DEFINES += QGNSS_BROM_LIBRARY

SOURCES += \
    $$PWD/Allystar/allystar_base.cpp \
    $$PWD/Allystar/allystar_upgrade.cpp \
    $$PWD/Broadcom/broadcom_base.cpp \
    $$PWD/Broadcom/broadcom_upgrade.cpp \
    $$PWD/Goke/goke_base.cpp \
    $$PWD/Goke/goke_upgrade.cpp \
    $$PWD/IO/iodvice.cpp   \
    $$PWD/IO/ioserial.cpp  \
    $$PWD/MTK/mtk_base.cpp \
    $$PWD/MTK/mtk_brom.cpp \
    $$PWD/MTK/mtk_brom_3335.cpp \
    $$PWD/MTK/mtk_brom_3352.cpp \
    $$PWD/Protocol/nmea_0813v14.cpp \
    $$PWD/Protocol/nmea_base.cpp \
    $$PWD/ST/st_mqsp_upgrade.cpp \
    $$PWD/ST/st_upgrade.cpp \
    $$PWD/Unicorecomm/unicorecomm_upgrade.cpp \
    $$PWD/brom_base.cpp \
    $$PWD/keyvalue.cpp \
    $$PWD/ST/st_base.cpp \
    $$PWD/Unicorecomm/unicorecomm_base.cpp \

HEADERS += \
    $$PWD/Allystar/allystar_base.h \
    $$PWD/Allystar/allystar_upgrade.h \
    $$PWD/Broadcom/broadcom_base.h \
    $$PWD/Broadcom/broadcom_upgrade.h \
    $$PWD/Goke/goke_base.h \
    $$PWD/Goke/goke_upgrade.h \
    $$PWD/IO/iodvice.h   \
    $$PWD/IO/ioserial.h  \
    $$PWD/MTK/mtk_base.h \
    $$PWD/MTK/mtk_brom.h \
    $$PWD/MTK/mtk_brom_3352.h \
    $$PWD/MTK/mtk_brom_3335.h \
    $$PWD/Protocol/nmea_0813v14.h \
    $$PWD/Protocol/nmea_base.h \
    $$PWD/QGNSS_BROM_global.h \
    $$PWD/ST/ST_BootLoader.h \
    $$PWD/ST/st_mqsp_upgrade.h \
    $$PWD/ST/st_mqsp_upgrade-inl.h \
    $$PWD/ST/st_upgrade.h \
    $$PWD/Unicorecomm/unicorecomm_upgrade.h \
    $$PWD/brom_base.h \
    $$PWD/keyvalue.h \
    $$PWD/ST/st_base.h \
    $$PWD/Unicorecomm/unicorecomm_base.h

INCLUDEPATH += \
           $$PWD \
           $$PWD/IO \
           $$PWD/MTK \
           $$PWD/Broadcom \
           $$PWD/Goke \
           $$PWD/Protocol \
           $$PWD/ST \
           $$PWD/Allystar \
           $$PWD/Unicorecomm




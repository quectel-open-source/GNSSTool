SOURCES += \
    #$$PWD/QGNSS_FW_UPG/QGNSS_FW_UPG.cpp \
    $$PWD/QGNSS_FW_UPG/QG_IO_Base.cpp \
    $$PWD/QGNSS_FW_UPG/QG_Print.cpp \
    $$PWD/QGNSS_FW_UPG/st/teseo5/ST8100_USART_download.cpp \
    $$PWD/QGNSS_FW_UPG/st/teseo5/ST8100_wmcu_fwupg.cpp \
    $$PWD/QGNSS_FW_UPG/st/teseo5/t5_wmcu_download_hp.cpp \
    $$PWD/QGNSS_FW_UPG/st/teseo5/t5_wmcu_upg_hp.cpp \
    $$PWD/QGNSS_FW_UPG/uart/QG_Uart_Base.cpp \
    $$PWD/QGNSS_FW_UPG/uart/QG_Uart_win.cpp \
    $$PWD/QG_Common.cpp \
    $$PWD/crc32.cpp

HEADERS += \
    #$$PWD/QGNSS_FW_UPG/QGNSS_FW_UPG.h \
    $$PWD/QGNSS_FW_UPG/QG_IO_Base.h \
    $$PWD/QGNSS_FW_UPG/QG_Print.h \
    $$PWD/QGNSS_FW_UPG/st/teseo5/ST8100_USART_download.h \
    $$PWD/QGNSS_FW_UPG/st/teseo5/ST8100_wmcu_fwupg.h \
    $$PWD/QGNSS_FW_UPG/st/teseo5/t5_wmcu_download_hp.h \
    $$PWD/QGNSS_FW_UPG/st/teseo5/t5_wmcu_upg_hp.h \
    $$PWD/QGNSS_FW_UPG/uart/QG_Uart.h \
    $$PWD/QGNSS_FW_UPG/uart/QG_Uart_Base.h \
    $$PWD/QG_Common.h \
    $$PWD/crc32.h

INCLUDEPATH += \
    $$PWD/QGNSS_FW_UPG \
    $$PWD \
    $$PWD/QGNSS_FW_UPG/st/teseo5 \
    $$PWD/QGNSS_FW_UPG/uart

 LIBS += -lAdvAPI32

 DEFINES += QG_OUTPUT_DEBUG

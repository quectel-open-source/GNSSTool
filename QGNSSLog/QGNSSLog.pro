QT       += core gui
QT += concurrent
QT += printsupport
QT += network

RC_ICONS = images/Qlog1.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

CONFIG += c++11


# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DEPRECATED_WARNINGS

#DEFINES += QCUSTOMPLOT_USE_OPENGL

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
#    NMEA_Analysis/NMEA_Analysis.cpp \
    FilterOptionsDialog.cpp \
    aboutdialog.cpp \
    infodialog.cpp \
    logplot_mainwindow.cpp \
    qcustomplot.cpp \
    qwaiting.cpp \
        main.cpp \

HEADERS += \
#    NMEA_Analysis/NMEA_Analysis.h \
    FilterOptionsDialog.h \
    aboutdialog.h \
    infodialog.h \
    logplot_mainwindow.h \
    qcustomplot.h \
    qwaiting.h \

FORMS += \
    FilterOptionsDialog.ui \
    aboutdialog.ui \
    infodialog.ui \
    logplot_mainwindow.ui \
    qwaiting.ui

RESOURCES += \
    image.qrc

#CONFIG += debug_and_release
CONFIG(debug, debug|release) {

}else{
     DESTDIR += $$PWD/../DevRuntimeEnv/
}

QNMEAPATH=$$PWD/../QNMEA_Build

DEFINES += USING_QNMEA_SHARED
win32:CONFIG(release, debug|release): LIBS += \
-L$$QNMEAPATH/lib/ -lQNMEA \
-L$$QNMEAPATH/bin/ -lQNMEA
else:win32:CONFIG(debug, debug|release): LIBS += \
-L$$QNMEAPATH/lib/ -lQNMEA_d  \
-L$$QNMEAPATH/bin/ -lQNMEA_d
#else:unix: LIBS += -L$$PWD/../build-QNMEA-Desktop_Qt_5_12_9_MSVC2017_64bit-Release/3rdpart/QNMEA/ -lQNMEA

INCLUDEPATH += $$PWD/../QNMEA/3rdpart/QNMEA/NMEA
INCLUDEPATH += $$PWD/../QNMEA/3rdpart/fmt/include
DEPENDPATH += $$PWD/../QNMEA/3rdpart/QNMEA/NMEA

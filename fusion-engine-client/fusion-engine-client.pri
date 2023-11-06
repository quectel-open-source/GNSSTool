HEADERS += \
    $$PWD/src/point_one/fusion_engine/common/logging.h \
    $$PWD/src/point_one/fusion_engine/common/portability.h \
    $$PWD/src/point_one/fusion_engine/common/version.h \
    $$PWD/src/point_one/fusion_engine/messages/configuration.h \
    $$PWD/src/point_one/fusion_engine/messages/control.h \
    $$PWD/src/point_one/fusion_engine/messages/core.h \
    $$PWD/src/point_one/fusion_engine/messages/crc.h \
    $$PWD/src/point_one/fusion_engine/messages/data_version.h \
    $$PWD/src/point_one/fusion_engine/messages/defs.h \
    $$PWD/src/point_one/fusion_engine/messages/device.h \
    $$PWD/src/point_one/fusion_engine/messages/fault_control.h \
    $$PWD/src/point_one/fusion_engine/messages/measurements.h \
    $$PWD/src/point_one/fusion_engine/messages/ros.h \
    $$PWD/src/point_one/fusion_engine/messages/signal_defs.h \
    $$PWD/src/point_one/fusion_engine/messages/solution.h \
    $$PWD/src/point_one/fusion_engine/parsers/fusion_engine_framer.h

SOURCES += \
    $$PWD/src/point_one/fusion_engine/common/logging.cc \
    $$PWD/src/point_one/fusion_engine/messages/crc.cc \
    $$PWD/src/point_one/fusion_engine/messages/data_version.cc \
    $$PWD/src/point_one/fusion_engine/parsers/fusion_engine_framer.cc

INCLUDEPATH += \
    $$PWD/src

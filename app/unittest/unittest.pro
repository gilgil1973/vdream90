#-------------------------------------------------
#
# Project created by QtCreator 2012-05-24T20:28:40
#
#-------------------------------------------------

QT -= gui

include (../../../../other/googletest/unittest.pri)
include (../../lib/vdream.pri)

TARGET    = unittest
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE  = app
DESTDIR   = ../../bin

SOURCES += \
    main.cpp \
    tcptest.cpp \
    ../../include/common/vapp.cpp \
    ../../include/common/vcommon.cpp \
    ../../include/common/vdesign.cpp \
    ../../include/common/verror.cpp \
    ../../include/common/vexception.cpp \
    ../../include/common/vfile.cpp \
    ../../include/common/vmetaclass.cpp \
    ../../include/common/vstring.cpp \
    ../../include/common/vsystem.cpp \
    ../../include/common/vthread.cpp \
    ../../include/common/vtick.cpp \
    ../../include/common/vxmldoc.cpp \
    ../../include/log/vlog.cpp \
    ../../include/net/vnet.cpp \
    ../../include/other/vint.cpp

HEADERS += \
    tcptest.h

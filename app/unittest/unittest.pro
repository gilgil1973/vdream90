#-------------------------------------------------
#
# Project created by QtCreator 2012-05-24T20:28:40
#
#-------------------------------------------------

QT -= gui

include(../../lib/vdream.pri)

TARGET    = unittest
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE  = app
DESTDIR   = ../../bin
DEFINES  +=_VARIADIC_MAX=10
DEFINES  +=GTEST

#-------------------------------------------------
# gtest
#-------------------------------------------------
GTEST_PATH = ../../../../other/googletest
INCLUDEPATH += $${GTEST_PATH}/include
win32:contains(QMAKE_TARGET.arch, x86_64) {
  win32:CONFIG(debug, debug|release) {
    LIBS += -L$${GTEST_PATH}/msvc/x64/Debug -lgtestd
  }
  win32:CONFIG(release, debug|release) {
    LIBS += -L$${GTEST_PATH}/msvc/x64/Release -lgtest
  }
} else {
  win32:CONFIG(debug, debug|release) {
    contains(QMAKE_CFLAGS_DEBUG, -MDd) {
      LIBS += -L$${GTEST_PATH}/msvc/gtest-md/Debug -lgtestd
    }
    contains(QMAKE_CFLAGS_DEBUG, -MTd) {
      LIBS += -L$${GTEST_PATH}/msvc/gtest/Debug -lgtestd
    }
  }
  win32:CONFIG(release, debug|release) {
    contains(QMAKE_CFLAGS_RELEASE, -MD) {
      LIBS += -L$${GTEST_PATH}/msvc/gtest-md/Release -lgtest
    }
    contains(QMAKE_CFLAGS_RELEASE, -MT) {
      LIBS += -L$${GTEST_PATH}/msvc/gtest/Release -lgtest
    }
  }
}
unix:LIBS += -L$${GTEST_PATH} -lgtest

SOURCES += \
    main.cpp \
    ../../include/common/vapp.cpp \
    ../../include/common/vcommon.cpp \
    ../../include/other/vint.cpp \
    ../../include/log/vlog.cpp \
    ../../include/common/vtick.cpp \
    ../../include/common/vdesign.cpp \
    ../../include/common/verror.cpp \
    ../../include/common/vexception.cpp \
    ../../include/common/vmetaclass.cpp \
    ../../include/common/vfile.cpp \
    ../../include/common/vthread.cpp \
    ../../include/net/vnet.cpp \
    ../../include/common/vstring.cpp \
    ../../include/common/vsystem.cpp \
    tcptest.cpp

HEADERS += \
    main.h \
    tcptest.h

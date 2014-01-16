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
    xmltest.cpp \
    ticktest.cpp \
    threadtest.cpp \
    tcptest.cpp \
    systemtest.cpp \
    stringtest.cpp \
    nettest.cpp \
    metaclasstest.cpp \
    main.cpp \
    logtest.cpp \
    inttest.cpp \
    filetest.cpp \
    exceptiontest.cpp \
    errortest.cpp \
    designtest.cpp \
    commontest.cpp \
    apptest.cpp

HEADERS += \
    tcptest.h \
    main.h

#-------------------------------------------------
#
# Project created by QtCreator 2012-05-24T20:39:05
#
#-------------------------------------------------

QT -= gui

include(../../lib/vdream.pri)

TARGET    = vdreamtest
CONFIG   += console
CONFIG   -= app_bundle
TEMPLATE  = app
DESTDIR   = ../../bin

SOURCES += \
    main.cpp \
    ../../include/net/vudpsession.cpp

HEADERS += \
    main.h

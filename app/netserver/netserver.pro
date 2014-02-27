#-------------------------------------------------
#
# Project created by QtCreator 2014-02-27T14:42:11
#
#-------------------------------------------------

QT += core gui widgets

include(../../lib/vdream.pri)

TARGET   = netserver
TEMPLATE = app
DESTDIR  = ../../bin

SOURCES += main.cpp\
        dialog.cpp

HEADERS  += dialog.h

FORMS    += dialog.ui

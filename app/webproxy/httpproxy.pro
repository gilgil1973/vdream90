#-------------------------------------------------
#
# Project created by QtCreator 2014-03-06T14:33:59
#
#-------------------------------------------------

QT       += core gui widgets

include(../../lib/vdream.pri)

TARGET = httpproxy
TEMPLATE = app
DESTDIR  = ../../bin

SOURCES += main.cpp\
        widget.cpp

HEADERS  += widget.h

FORMS    += widget.ui

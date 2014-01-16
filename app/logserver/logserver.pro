#-------------------------------------------------
#
# Project created by QtCreator 2012-06-03T02:26:37
#
#-------------------------------------------------

QT += core gui
QT += widgets

include(../../lib/vdream.pri)

TARGET        = logserver
TEMPLATE      = app
DESTDIR       = ../../bin
win32:LIBS   += -lshell32
win32:RC_FILE = logserver.rc

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    optiondlg.cpp \
    aboutdlg.cpp \
    mylog.cpp

HEADERS += \
    mainwindow.h \
    optiondlg.h \
    aboutdlg.h \
    mylog.h

FORMS += \
    mainwindow.ui \
    optiondlg.ui \
    aboutdlg.ui

RESOURCES += \
    logserver.qrc

QT += core xml

#-------------------------------------------------
# VDREAM_LIB_NAME
#-------------------------------------------------
VDREAM_LIB_NAME=vdream
CONFIG(debug, debug|release) {
  VDREAM_LIB_NAME=$${VDREAM_LIB_NAME}d
}

contains(QT, gui) {
  VDREAM_LIB_NAME=$${VDREAM_LIB_NAME}_gui
} else {
}

message(VDREAM_LIB_NAME=$${VDREAM_LIB_NAME}) # gilgil temp 2013.04.15

#-------------------------------------------------
# debug and release
#-------------------------------------------------
CONFIG(debug, debug|release) {
  DEFINES += _DEBUG
}
CONFIG(release, debug|release) {
  DEFINES += _RELEASE
}

#-------------------------------------------------
# openssl
#-------------------------------------------------
win32:contains(QMAKE_TARGET.arch, x86_64) {
  OPENSSL_PATH = C:/OpenSSL-Win64
} else {
  OPENSSL_PATH = C:/OpenSSL-Win32
}
INCLUDEPATH   += $${OPENSSL_PATH}/include
LIBS          += -L$${OPENSSL_PATH}/lib -llibeay32 -lssleay32

#-------------------------------------------------
# vdream
#-------------------------------------------------
VDREAM_PATH            =   $$PWD/..
INCLUDEPATH           +=   $${VDREAM_PATH}/include
DEPENDPATH            +=   $${VDREAM_PATH}
LIBS                  += -L$${VDREAM_PATH}/lib -l$${VDREAM_LIB_NAME}
CONFIG(VDREAM_LIB_BUILD) {
} else {
  win32:PRE_TARGETDEPS +=   $${VDREAM_PATH}/lib/$${VDREAM_LIB_NAME}.lib
  unix: PRE_TARGETDEPS +=   $${VDREAM_PATH}/lib/lib$${VDREAM_LIB_NAME}.a
}

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

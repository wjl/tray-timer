QT       += core gui
CONFIG   += debug
TARGET    = tray-timer
TEMPLATE  = app

QMAKE_CFLAGS   = -std=c++11 -pthread -fPIC
QMAKE_CXXFLAGS = -std=c++11 -pthread -fPIC

HEADERS += tray-timer.h++
SOURCES += main.c++ tray-timer.c++

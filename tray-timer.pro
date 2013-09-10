QT       += core gui widgets
CONFIG   += debug
TARGET    = tray-timer
TEMPLATE  = app

QMAKE_CXXFLAGS = -std=c++11 -pthread -fPIC
QMAKE_EXT_CPP += .c++
QMAKE_EXT_H   += .h++

HEADERS += tray-timer.h++
SOURCES += main.c++ tray-timer.c++

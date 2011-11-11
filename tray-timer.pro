QT       += core gui
CONFIG   += debug
TARGET    = tray-timer
TEMPLATE  = app

QMAKE_CFLAGS   = -std=c++0x -pthread -fPIC
QMAKE_CXXFLAGS = -std=c++0x -pthread -fPIC

HEADERS += tray-timer.h++
SOURCES += main.c++ tray-timer.c++

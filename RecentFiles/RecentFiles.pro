#-------------------------------------------------
#
# Project created by QtCreator 2016-03-12T22:30:27
#
#-------------------------------------------------

QT += core gui
QT += xml
QMAKE_CXXFLAGS += -Wno-deprecated-copy

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = RecentFiles
TEMPLATE = app
QMAKE_CXXFLAGS += -std=c++17

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui


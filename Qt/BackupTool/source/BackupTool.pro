#-------------------------------------------------
#
# Project created by QtCreator 2018-10-06T22:38:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

unix:QMAKE_CXXFLAGS += -std=c++17

unix:QMAKE_CFLAGS_WARN_ON += -Wno-expansion-to-defined
unix:QMAKE_CXXFLAGS_WARN_ON += -Wno-expansion-to-defined

unix:QMAKE_CFLAGS_WARN_ON += -Wno-deprecated-copy
unix:QMAKE_CXXFLAGS_WARN_ON += -Wno-deprecated-copy

TARGET = BackupTool
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    helper.cpp \
    logger.cpp \
    workerthreadconnector.cpp

HEADERS  += mainwindow.h \
    helper.h \
    logger.h \
    workerthreadconnector.h

FORMS    += mainwindow.ui

win32:INCLUDEPATH = E:/Programmierung/lib/boost_1_68_0

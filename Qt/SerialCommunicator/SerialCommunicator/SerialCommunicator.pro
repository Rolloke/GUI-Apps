#-------------------------------------------------
#
# Project created by QtCreator 2018-03-17T19:14:31
#
#-------------------------------------------------

QT       += core gui


greaterThan(QT_MAJOR_VERSION, 4): QT += serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

unix:QMAKE_CXXFLAGS += -std=c++17
unix:QMAKE_CXXFLAGS += -Wno-deprecated-copy

unix:QMAKE_CFLAGS_WARN_ON += -Wno-expansion-to-defined
unix:QMAKE_CXXFLAGS_WARN_ON += -Wno-expansion-to-defined

TARGET = SerialCommunicator
TEMPLATE = app


SOURCES += main.cpp\
    mainwindow.cpp \
    logger.cpp \
    pintype.cpp \
    inputpin.cpp \
    outputpin.cpp \
    pincommon.cpp

lessThan(QT_MAJOR_VERSION, 5): SOURCES += serialport.cpp

HEADERS  += mainwindow.h \
    logger.h \
    pintype.h \
    inputpin.h \
    outputpin.h \
    pincommon.h

lessThan(QT_MAJOR_VERSION, 5): HEADERS += serialport.h

FORMS    += mainwindow.ui

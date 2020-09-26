#-------------------------------------------------
#
# Project created by QtCreator 2016-05-12T20:53:25
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = syslogger
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++14
QMAKE_CXXFLAGS += -Wno-unused-function
QMAKE_CXXFLAGS += -Wno-deprecated-copy

SOURCES += main.cpp\
        receiverthread.cpp\
	sysloggerwidget.cpp

HEADERS  += receiverthread.h\
	sysloggerwidget.h\
	asyncqueue.h

FORMS    += sysloggerwidget.ui

#-------------------------------------------------
#
# Project created by QtCreator 2016-05-12T20:53:25
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = syslogger
TEMPLATE = app

unix:QMAKE_CXXFLAGS += -std=c++17
unix:QMAKE_CXXFLAGS += -Wno-unused-function
unix:QMAKE_CXXFLAGS += -Wno-deprecated-copy
unix:QMAKE_CFLAGS_WARN_ON += -Wno-expansion-to-defined
unix:QMAKE_CXXFLAGS_WARN_ON += -Wno-expansion-to-defined

SOURCES += main.cpp\
        receiverthread.cpp\
	sysloggerwidget.cpp

HEADERS  += receiverthread.h\
	sysloggerwidget.h\
	asyncqueue.h

FORMS    += sysloggerwidget.ui

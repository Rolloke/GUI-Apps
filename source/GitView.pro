#-------------------------------------------------
#
# Project created by QtCreator 2018-10-06T22:38:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++14

TARGET = GitView
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    helper.cpp \
    logger.cpp \
    workerthreadconnector.cpp \
    git_type.cpp

HEADERS  += mainwindow.h \
    helper.h \
    logger.h \
    workerthreadconnector.h \
    git_type.h

FORMS    += mainwindow.ui

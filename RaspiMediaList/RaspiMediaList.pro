#-------------------------------------------------
#
# Project created by QtCreator 2016-03-12T22:30:27
#
#-------------------------------------------------

QT += core gui
QT += xml
QT += multimedia
QT += multimediawidgets
#QT += webenginewidgets webchannel
#DEFINES += WEB_ENGINE=1

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET = RaspiMediaList
TEMPLATE = app

CONFIG += c++17
unix:QMAKE_CXXFLAGS += -std=c++17
unix:QMAKE_LFLAGS   = -fuse-ld=gold
unix:QMAKE_CFLAGS_WARN_ON += -Wno-expansion-to-defined
unix:QMAKE_CFLAGS_WARN_ON += -Wno-deprecated-copy
unix:QMAKE_CXXFLAGS_WARN_ON += -Wno-expansion-to-defined
unix:QMAKE_CXXFLAGS_WARN_ON += -Wno-deprecated-copy

win32:INCLUDEPATH = E:/Programmierung/lib/boost_1_68_0

SOURCES += main.cpp \
        mainwindow.cpp \
        logger.cpp

HEADERS  += mainwindow.h \
         logger.h

FORMS    += mainwindow.ui

RESOURCES += resource/resource.qrc
RC_FILE = resource/resource.rc
win32:RC_ICONS = :/36x36/applications-multimedia.ico

TRANSLATIONS    = RaspiMediaList_de.ts


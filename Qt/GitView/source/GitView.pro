#-------------------------------------------------
#
# Project created by QtCreator 2018-10-06T22:38:45
#
#-------------------------------------------------

QT += core gui
QT += xml
QT += svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


#DEFINES += NDEBUG


TARGET = GitView
TEMPLATE = app

CONFIG += c++17
unix:QMAKE_CXXFLAGS += -std=c++17
unix:QMAKE_CFLAGS_WARN_ON += -Wno-expansion-to-defined
unix:QMAKE_CFLAGS_WARN_ON += -Wno-deprecated-copy
unix:QMAKE_CXXFLAGS_WARN_ON += -Wno-expansion-to-defined
unix:QMAKE_CXXFLAGS_WARN_ON += -Wno-deprecated-copy

win32:INCLUDEPATH = E:/Programmierung/lib/boost_1_68_0

SOURCES += main.cpp\
    code_browser.cpp \
    highlighterdialog.cpp \
    mainwindow.cpp \
    helper.cpp \
    logger.cpp \
    mainwindow_branchtree.cpp \
    mainwindow_historytree.cpp \
    mainwindow_sourcetree.cpp \
    mainwindow_stashtree.cpp \
    mainwindow_textview.cpp \
    workerthreadconnector.cpp \
    git_type.cpp \
    commitmessage.cpp \
    history.cpp \
    actions.cpp \
    customgitactions.cpp \
    qbranchtreewidget.cpp \
    qhistorytreewidget.cpp \
    qstashtreewidget.cpp \
    gitignore.cpp \
    aboutdlg.cpp \
    highlighter.cpp \
    xml_functions.cpp \
    mergedialog.cpp \
    qradiobuttongroup.cpp \
    graphics_view.cpp

HEADERS  += mainwindow.h \
    code_browser.h \
    helper.h \
    highlighterdialog.h \
    logger.h \
    workerthreadconnector.h \
    git_type.h \
    commitmessage.h \
    history.h \
    actions.h \
    customgitactions.h \
    qbranchtreewidget.h \
    qhistorytreewidget.h \
    qstashtreewidget.h \
    gitignore.h \
    aboutdlg.h \
    highlighter.h \
    xml_functions.h \
    mergedialog.h \
    qradiobuttongroup.h \
    graphics_view.h


FORMS    += mainwindow.ui \
    commitmessage.ui \
    customgitactions.ui \
    aboutdlg.ui \
    highlighterdialog.ui \
    mergedialog.ui

RESOURCES += resource.qrc
RC_FILE = resource/resource.rc

TRANSLATIONS    = GitView_de.ts

#RC_ICONS = :/resource/logo@2x.ico

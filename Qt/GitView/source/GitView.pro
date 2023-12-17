#-------------------------------------------------
#
# Project created by QtCreator 2018-10-06T22:38:45
#
#-------------------------------------------------



QT += core gui
QT += xml
QT += svg
greaterThan(QT_MAJOR_VERSION, 5): QT += svgwidgets

QT += webenginewidgets webchannel
DEFINES += WEB_ENGINE=1

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
#greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

TARGET = GitView
TEMPLATE = app

CONFIG += c++17
CONFIG -= qtquickcompiler

unix:QMAKE_CXXFLAGS += -std=c++17
unix:QMAKE_LFLAGS   = -fuse-ld=gold
unix:QMAKE_CFLAGS_WARN_ON += -Wno-expansion-to-defined
unix:QMAKE_CFLAGS_WARN_ON += -Wno-deprecated-copy
unix:QMAKE_CXXFLAGS_WARN_ON += -Wno-expansion-to-defined
unix:QMAKE_CXXFLAGS_WARN_ON += -Wno-deprecated-copy

win32:INCLUDEPATH = E:/Programmierung/lib/boost_1_68_0

SOURCES += main.cpp\
    binary_values_view.cpp \
    code_browser.cpp \
    commit_graphics_item.cpp \
    editable.cpp \
    highlighterdialog.cpp \
    mainwindow.cpp \
    helper.cpp \
    logger.cpp \
    mainwindow_branchtree.cpp \
    mainwindow_historytree.cpp \
    mainwindow_sourcetree.cpp \
    mainwindow_stashtree.cpp \
    mainwindow_textview.cpp \
    palettecolorselector.cpp \
    qdockwidgetx.cpp \
    qsourcetreewidget.cpp \
    workerthreadconnector.cpp \
    git_type.cpp \
    commitmessage.cpp \
    history.cpp \
    actions.cpp \
    customgitactions.cpp \
    qbranchtreewidget.cpp \
    qhistorytreewidget.cpp \
    qstashtreewidget.cpp \
    qbinarytableview.cpp \
    gitignore.cpp \
    aboutdlg.cpp \
    highlighter.cpp \
    xml_functions.cpp \
    mergedialog.cpp \
    qradiobuttongroup.cpp \
    graphics_view.cpp \
    DisplayType.cpp

HEADERS  += mainwindow.h \
    binary_values_view.h \
    code_browser.h \
    commit_graphics_item.h \
    editable.h \
    helper.h \
    highlighterdialog.h \
    logger.h \
    palettecolorselector.h \
    qdockwidgetx.h \
    qsourcetreewidget.h \
    workerthreadconnector.h \
    git_type.h \
    commitmessage.h \
    history.h \
    actions.h \
    customgitactions.h \
    qbranchtreewidget.h \
    qhistorytreewidget.h \
    qstashtreewidget.h \
    qbinarytableview.h \
    gitignore.h \
    aboutdlg.h \
    highlighter.h \
    xml_functions.h \
    mergedialog.h \
    qradiobuttongroup.h \
    graphics_view.h \
    DisplayType.h


FORMS    += mainwindow.ui \
    binary_values_view.ui \
    commitmessage.ui \
    customgitactions.ui \
    aboutdlg.ui \
    highlighterdialog.ui \
    mergedialog.ui \
    palettecolorselector.ui

RESOURCES += resource.qrc
RC_FILE = resource/resource.rc
win32:RC_ICONS = :/resource/logo@2x.ico

TRANSLATIONS    = GitView_de.ts


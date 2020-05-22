#-------------------------------------------------
#
# Project created by QtCreator 2018-10-06T22:38:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets


#DEFINES += NDEBUG


TARGET = GitView
TEMPLATE = app

unix:QMAKE_CFLAGS_WARN_ON += -Wno-expansion-to-defined
unix:QMAKE_CXXFLAGS_WARN_ON += -Wno-expansion-to-defined


SOURCES += main.cpp\
        mainwindow.cpp \
    helper.cpp \
    logger.cpp \
    workerthreadconnector.cpp \
    git_type.cpp \
    commitmessage.cpp \
    history.cpp \
    actions.cpp \
    customgitactions.cpp \
    qbranchtreewidget.cpp \
    qhistorytreewidget.cpp

HEADERS  += mainwindow.h \
    helper.h \
    logger.h \
    workerthreadconnector.h \
    git_type.h \
    commitmessage.h \
    history.h \
    actions.h \
    customgitactions.h \
    qbranchtreewidget.h \
    qhistorytreewidget.h

FORMS    += mainwindow.ui \
    commitmessage.ui \
    customgitactions.ui

RESOURCES += resource.qrc

#-------------------------------------------------
#
# Project created by QtCreator 2014-05-03T21:35:56
#
#-------------------------------------------------

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += multimedia

QT += core gui
QT += xml

QMAKE_CXXFLAGS += -std=c++17
#CONFIG(release):
QMAKE_CXXFLAGS += -O3

TARGET = Schrittmotor
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    PlotterDriver.cpp \
    LPTPort.cpp \
    PlotterDC.cpp \
    PostscriptInterpreter.cpp \
    calibrateplotter.cpp \
    hpglinterpreter.cpp \
    commonFunctions.cpp \
    xml_functions.cpp \
    lptporttestdlg.cpp \
    gcodeinterpreter.cpp \
    gcodecontroldlg.cpp \
    gbr_interpreter.cpp \
    pointf.cpp \
    plotterdriverconnection.cpp \
    carithmetic.cpp

HEADERS  += mainwindow.h \
    PlotterDriver.h \
    PlotterDC.h \
    LPTPort.h \
    matrix.h \
    PostscriptInterpreter.h \
    defines.h \
    calibrateplotter.h \
    hpglinterpreter.h \
    commonFunctions.h \
    xml_functions.h \
    lptporttestdlg.h \
    main.h \
    gcodeinterpreter.h \
    gcodecontroldlg.h \
    gbr_interpreter.h \
    pointf.h \
    plotterdriverconnection.h \
    carithmetic.h

FORMS    += mainwindow.ui \
    calibrateplotter.ui \
    lptporttestdlg.ui \
    gcodecontroldlg.ui

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../usr/lib/release/ -lboost_serialization
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../usr/lib/debug/ -lboost_serialization
#else:symbian: LIBS += -lboost_serialization
#else:unix: LIBS += -L/usr/lib/ -lboost_serialization

INCLUDEPATH += $$PWD/../../../../../../usr/include
DEPENDPATH += $$PWD/../../../../../../usr/include

#win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../usr/lib/release/boost_serialization.lib
#else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../usr/lib/debug/boost_serialization.lib
#else:unix:!symbian: PRE_TARGETDEPS += /usr/lib/libboost_serialization.a

RESOURCES += \
    resource.qrc

OTHER_FILES += ./images/plotter.png \
                applicationicon.desktop
symbian {
     include($$QT_SOURCE_TREE/examples/symbianpkgrules.pri)
     # override icon
     ICON = ./images/plotter.png
     TARGET.UID3 = 0xe9f919ee
     TARGET.EPOCSTACKSIZE = 0x14000
     TARGET.EPOCHEAPSIZE = 0x020000 0x800000
 }

unix|win32: LIBS += -lieee1284 -lboost_regex
#-lboost_thread -lboost_system

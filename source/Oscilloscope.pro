#-------------------------------------------------
#
# Project created by QtCreator 2014-10-22T20:09:07
#
#-------------------------------------------------

QT += core gui
QT += xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 4): QT += multimedia

lessThan(QT_MAJOR_VERSION, 5):TARGET = Oscilloscope
greaterThan(QT_MAJOR_VERSION, 4):TARGET = OscilloscopeQt5

TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++17
QMAKE_CXXFLAGS += -msse2
#QMAKE_CXXFLAGS += -mavx

QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_SSE2
QMAKE_CXXFLAGS += -Wno-deprecated-copy

QMAKE_CFLAGS_WARN_ON += -Wno-expansion-to-defined
QMAKE_CXXFLAGS_WARN_ON += -Wno-expansion-to-defined

DEFINES +=__SSE2__
#DEFINES +=__AVX__


# define trigger processing:
# 1: processing depending on buffer update flag
# 2: find all trigger points and take the latest trigger point with enough display poinds behind it
#    note: processing 2 is not stable
DEFINES += TRIGGER_PROCESSING=1

# use Qt Audio instead of portaudio
greaterThan(QT_MAJOR_VERSION, 4): DEFINES += _USE_QT_AUDIO=1

SOURCES += main.cpp\
        mainwindow.cpp \
    scopeview.cpp \
    qaudiorecord.cpp \
    Channel.cpp \
    xml_functions.cpp \
    PolyLineItem.cpp \
    FFT.cpp \
    calibration.cpp \
    fft_view.cpp \
    audiosettings.cpp \
    scopesettings.cpp \
    cursors.cpp \
    qradiobuttongroup.cpp \
    functiongenerator.cpp \
    generatefunction.cpp \
    qgroupeditwithunit.cpp \
    unitprefix.cpp \
    qdoublelablewithunit.cpp \
    pseudorandomgenerator.cpp \
    filterfunctions.cpp \
    TriggerParameter.cpp \
    settings.cpp \
    qaudioparameter.cpp \
    combinecurves.cpp \
    qcomboboxdelegate.cpp \
    memorypool.cpp \
    qwidgetitemdelegate.cpp \
    DirectIoDevice.cpp \
    logger.cpp \
    workerthreadconnector.cpp \
    qaudiooutstream.cpp \
    sse_helper.cpp \
    events.cpp \
    aboutdlg.cpp

HEADERS  += mainwindow.h \
    scopeview.h \
    qaudiorecord.h \
    Channel.h \
    xml_functions.h \
    PolyLineItem.h \
    FFT.h \
    calibration.h \
    Average.h \
    fft_view.h \
    main.h \
    audiosettings.h \
    scopesettings.h \
    cursors.h \
    qradiobuttongroup.h \
    functiongenerator.h \
    generatefunction.h \
    data_exchange.h \
    qgroupeditwithunit.h \
    unitprefix.h \
    qdoublelablewithunit.h \
    pseudorandomgenerator.h \
    filterfunctions.h \
    TriggerParameter.h \
    settings.h \
    qaudioparameter.h \
    combinecurves.h \
    circlevector.h \
    qcomboboxdelegate.h \
    memorypool.h \
    qwidgetitemdelegate.h \
    DirectIoDevice.h \
    logger.h \
    workerthreadconnector.h \
    qaudiooutstream.h \
    matrix.h \
    functions.h \
    sse_helper.h \
    events.h \
    aboutdlg.h

FORMS    += mainwindow.ui \
    calibration.ui \
    audiosettings.ui \
    scopesettings.ui \
    functiongenerator.ui \
    aboutdlg.ui



win32:INCLUDEPATH = E:/Programmierung/lib/boost_1_68_0

#INCLUDEPATH += /usr/include
#DEPENDPATH += /usr/include

#INCLUDEPATH += /usr/lib/i386-linux-gnu
#DEPENDPATH += /usr/lib/i386-linux-gnu


#unix:!macx:!symbian: LIBS += -L/usr/lib/ -lportaudio
unix:!macx:!symbian: LIBS += -L/home/rolf/Projekte/portaudio/lib/.libs

lessThan(QT_MAJOR_VERSION, 5):  LIBS += -lportaudio

#isEmpty(QMAKE_CFLAGS_AVX):error("This compiler does not support AVX")
#else:QMAKE_CXXFLAGS += $$QMAKE_CFLAGS_AVX

#QMAKE_CXXFLAGS_DEBUG -= -O3
#QMAKE_CXXFLAGS_DEBUG -= -O1
#QMAKE_CXXFLAGS_DEBUG -= -O2
#QMAKE_CXXFLAGS_DEBUG += -O

QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3

# does not work properly
#QMAKE_CXXFLAGS += -mavx

RC_FILE = resource/resource.rc

RESOURCES += resource.qrc

DISTFILES += doxygen.config


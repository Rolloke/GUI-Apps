#-------------------------------------------------
#
# Project created by QtCreator 2019-04-30T17:58:23
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++14

TARGET = ArduinoUnittest
TEMPLATE = app



SOURCES += main.cpp \
           mainwindow.cpp \
           Arduino.cpp \
           serialinterface.cpp \
           WString.cpp \
           EEPROM.cpp \
           Print.cpp \
           LiquidCrystal.cpp \
           Stream.cpp \
           Wire.cpp

HEADERS  += mainwindow.h \
            Arduino.h \
            serialinterface.h \
            WString.h \
            EEPROM.h \
            WProgram.h \
            unittest.h \
            Print.h \
            Printable.h \
            LiquidCrystal.h \
            Stream.h \
            Wire.h

# Aduino Project Files
SKETCH_NAME = OctansSimulatorDisplay
ARDUINO_PATH = ../../../Arduino/Sketches/OctansSimulator/$${SKETCH_NAME}
INCLUDEPATH += $${ARDUINO_PATH}
SOURCES += OctansSimulatorino.cpp
DISTFILES += $${ARDUINO_PATH}/$${SKETCH_NAME}.ino

# Libraries
LIBRARY1 = /opt/arduino-1.8.9/libraries/TextTools/src
INCLUDEPATH += $${LIBRARY1}
SOURCES += $${LIBRARY1}/texttools.cpp
HEADERS += $${LIBRARY1}/texttools.h

LIBRARY2 = /opt/arduino-1.8.9/libraries/Time
INCLUDEPATH += $${LIBRARY2}
SOURCES += $${LIBRARY2}/Time.cpp
SOURCES += $${LIBRARY2}/DateStrings.cpp
HEADERS += $${LIBRARY2}/Time.h

# Libraries
LIBRARY3 = /opt/arduino-1.8.9/libraries/Button/src
INCLUDEPATH += $${LIBRARY3}
SOURCES += $${LIBRARY3}/button.cpp
HEADERS += $${LIBRARY3}/button.h

LIBRARY4 = /opt/arduino-1.8.9/libraries/TextMenu/src
INCLUDEPATH += $${LIBRARY4}
SOURCES += $${LIBRARY4}/menu.cpp
HEADERS += $${LIBRARY4}/menu.h

FORMS    += mainwindow.ui


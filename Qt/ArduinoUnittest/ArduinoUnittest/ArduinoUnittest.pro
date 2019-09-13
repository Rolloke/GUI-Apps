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

ARDUINO_PATH = ../../../Arduino/Sketches/ConfigurableDelayTimer
# Aduino Project Files
INCLUDEPATH += $${ARDUINO_PATH}
HEADERS += $${ARDUINO_PATH}/Input.h
HEADERS += $${ARDUINO_PATH}/Output.h
SOURCES += configurabledelaytimerino.cpp
DISTFILES += $${ARDUINO_PATH}/ConfigurableDelayTimer.ino
SOURCES += $${ARDUINO_PATH}/Input.cpp
SOURCES += $${ARDUINO_PATH}/Output.cpp

# Libraries
LIBRARY1 = /opt/arduino-1.8.9/libraries/TextTools/src
INCLUDEPATH += $${LIBRARY1}
SOURCES += $${LIBRARY1}/texttools.cpp
HEADERS += $${LIBRARY1}/texttools.h


FORMS    += mainwindow.ui

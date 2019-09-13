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
ARDUINO_PATH = ../../../Arduino/Sketches/FunctionGenerator
INCLUDEPATH += $${ARDUINO_PATH}
SOURCES += FunctionGeneratorIno.cpp
SOURCES += $${ARDUINO_PATH}/AD9833.cpp
HEADERS += $${ARDUINO_PATH}/AD9833.h
HEADERS += $${ARDUINO_PATH}/ioATMegaNano.h
HEADERS += $${ARDUINO_PATH}/LCD_print.h
DISTFILES += $${ARDUINO_PATH}/FunctionGenerator.ino

# Libraries
LIBRARY1 = /opt/arduino-1.8.9/libraries/Button/src
INCLUDEPATH += $${LIBRARY1}
SOURCES += $${LIBRARY1}/button.cpp
HEADERS += $${LIBRARY1}/button.h

LIBRARY2 = /opt/arduino-1.8.9/libraries/TextMenu/src
INCLUDEPATH += $${LIBRARY2}
SOURCES += $${LIBRARY2}/menu.cpp
HEADERS += $${LIBRARY2}/menu.h


FORMS    += mainwindow.ui

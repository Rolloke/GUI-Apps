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
ARDUINO_PATH = ../../../Arduino/Sketches/Uhren/BinaerAnalogUhr/binaer_analog_uhr
INCLUDEPATH += $${ARDUINO_PATH}
HEADERS += $${ARDUINO_PATH}/BinaryClockMux.h
HEADERS += $${ARDUINO_PATH}/Melody.h
HEADERS += $${ARDUINO_PATH}/SettingStates.h
HEADERS += $${ARDUINO_PATH}/big_ben.h
HEADERS += $${ARDUINO_PATH}/die_maus.h
HEADERS += $${ARDUINO_PATH}/ioAt328.h
HEADERS += $${ARDUINO_PATH}/piep.h
HEADERS += $${ARDUINO_PATH}/SevenTone.h

SOURCES += binaer_analog_uhr.cpp
DISTFILES += $${ARDUINO_PATH}/binaer_analog_uhr.ino
SOURCES += $${ARDUINO_PATH}/BinaryClockMux.cpp
SOURCES += $${ARDUINO_PATH}/Melody.cpp
SOURCES += $${ARDUINO_PATH}/SettingStates.cpp

# Libraries
LIBRARY1 = /opt/arduino-1.8.9/libraries/Button/src
INCLUDEPATH += $${LIBRARY1}
SOURCES += $${LIBRARY1}/button.cpp
HEADERS += $${LIBRARY1}/button.h

LIBRARY2 = /opt/arduino-1.8.9/libraries/Time
INCLUDEPATH += $${LIBRARY2}
SOURCES += $${LIBRARY2}/Time.cpp
HEADERS += $${LIBRARY2}/Time.h
SOURCES += $${LIBRARY2}/DateStrings.cpp

LIBRARY3 = /opt/arduino-1.8.9/libraries/TimeAlarms
INCLUDEPATH += $${LIBRARY3}
SOURCES += $${LIBRARY3}/TimeAlarms.cpp
HEADERS += $${LIBRARY3}/TimeAlarms.h

LIBRARY4 = /opt/arduino-1.8.9/libraries/DS1307RTC
INCLUDEPATH += $${LIBRARY4}
SOURCES += $${LIBRARY4}/DS1307RTC.cpp
HEADERS += $${LIBRARY4}/DS1307RTC.h


FORMS    += mainwindow.ui

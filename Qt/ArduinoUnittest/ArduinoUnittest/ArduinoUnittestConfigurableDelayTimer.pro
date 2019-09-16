#-------------------------------------------------
#
# Project created by QtCreator 2019-04-30T17:58:23
#
#-------------------------------------------------

include (ArduinoUnittest.pro)

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
LIBRARY1 = $${ARDUINO_LOCAL_LIBRARY_PATH}/TextTools/src
INCLUDEPATH += $${LIBRARY1}
SOURCES += $${LIBRARY1}/texttools.cpp
HEADERS += $${LIBRARY1}/texttools.h

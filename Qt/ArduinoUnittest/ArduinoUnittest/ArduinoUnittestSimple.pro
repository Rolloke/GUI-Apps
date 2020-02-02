#-------------------------------------------------
#
# Project created by QtCreator 2019-04-30T17:58:23
#
#-------------------------------------------------
include (ArduinoUnittest.pro)

ARDUINO_PATH = /home/rolf/Projekte/Arduino/Sketches/test/stepper
# Aduino Project Files
DISTFILES += $${ARDUINO_PATH}/stepper.ino
SOURCES += StepperIno.cpp

# Libraries
LIBRARY1 = $${ARDUINO_LOCAL_LIBRARY_PATH}/Stepper/src
INCLUDEPATH += $${LIBRARY1}
SOURCES += $${LIBRARY1}/Stepper.cpp
HEADERS += $${LIBRARY1}/Stepper.h


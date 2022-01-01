#-------------------------------------------------
#
# Project created by QtCreator 2019-04-30T17:58:23
#
#-------------------------------------------------

include (ArduinoUnittest.pro)

EMULATED=1
DEFINES += EMULATED

# Aduino Project Files
SKETCH_NAME = AlarmLeuchte
ARDUINO_PATH = /home/rolf/projects/Arduino/Sketches/$${SKETCH_NAME}
INCLUDEPATH += $${ARDUINO_PATH}
SOURCES += $${SKETCH_NAME}.cpp
DISTFILES += $${ARDUINO_PATH}/$${SKETCH_NAME}.ino

INO_FILE="$${ARDUINO_PATH}/$${SKETCH_NAME}.ino"
DEFINES += INO_FILE


# Libraries

LIBRARY1 = $${ARDUINO_LOCAL_LIBRARY_PATH}/Button/src
INCLUDEPATH += $${LIBRARY1}
SOURCES += $${LIBRARY1}/button.cpp
HEADERS += $${LIBRARY1}/button.h

LIBRARY2 = $${ARDUINO_LOCAL_LIBRARY_PATH}/PinController/src
INCLUDEPATH += $${LIBRARY2}
SOURCES += $${LIBRARY2}/PinController.cpp
HEADERS += $${LIBRARY2}/PinController.h

LIBRARY3 = $${ARDUINO_LOCAL_LIBRARY_PATH}/Melody/src
INCLUDEPATH += $${LIBRARY3}
SOURCES += $${LIBRARY3}/Melody.cpp
HEADERS += $${LIBRARY3}/Melody.h

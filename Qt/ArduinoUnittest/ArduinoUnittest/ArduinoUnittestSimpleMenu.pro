#-------------------------------------------------
#
# Project created by QtCreator 2019-04-30T17:58:23
#
#-------------------------------------------------

include (ArduinoUnittest.pro)

# Aduino Project Files
SKETCH_NAME = SimpleMenu1
ARDUINO_PATH = $${ARDUINO_LOCAL_LIBRARY_PATH}/TextMenu/examples/$${SKETCH_NAME}
INCLUDEPATH += $${ARDUINO_PATH}
SOURCES += SimpleMenuino.cpp
DISTFILES += $${ARDUINO_PATH}/$${SKETCH_NAME}.ino


# Libraries

LIBRARY1 = $${ARDUINO_LOCAL_LIBRARY_PATH}/Button/src
INCLUDEPATH += $${LIBRARY1}
SOURCES += $${LIBRARY1}/button.cpp
HEADERS += $${LIBRARY1}/button.h

LIBRARY2 = $${ARDUINO_LOCAL_LIBRARY_PATH}/TextMenu/src
INCLUDEPATH += $${LIBRARY2}
SOURCES += $${LIBRARY2}/menu.cpp
HEADERS += $${LIBRARY2}/menu.h


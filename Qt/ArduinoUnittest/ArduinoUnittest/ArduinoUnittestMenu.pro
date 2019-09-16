#-------------------------------------------------
#
# Project created by QtCreator 2019-04-30T17:58:23
#
#-------------------------------------------------
include (ArduinoUnittest.pro)

ARDUINO_PATH = $${ARDUINO_LOCAL_LIBRARY_PATH}/TextMenu/examples/SimpleMenu
# Aduino Project Files
SOURCES += SimpleMenu.cpp
DISTFILES += $${ARDUINO_PATH}/SimpleMenu.ino

# Libraries
LIBRARY1 = $${ARDUINO_LOCAL_LIBRARY_PATH}/TextMenu/src
INCLUDEPATH += $${LIBRARY1}
SOURCES += $${LIBRARY1}/menu.cpp
HEADERS += $${LIBRARY1}/menu.h

# Libraries
LIBRARY2 = $${ARDUINO_LOCAL_LIBRARY_PATH}/Button/src
INCLUDEPATH += $${LIBRARY2}
SOURCES += $${LIBRARY2}/button.cpp
HEADERS += $${LIBRARY2}/button.h

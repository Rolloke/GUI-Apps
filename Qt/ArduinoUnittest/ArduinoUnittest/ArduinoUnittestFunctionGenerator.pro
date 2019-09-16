include (ArduinoUnittest.pro)

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
LIBRARY1 = $${ARDUINO_LOCAL_LIBRARY_PATH}/Button/src
INCLUDEPATH += $${LIBRARY1}
SOURCES += $${LIBRARY1}/button.cpp
HEADERS += $${LIBRARY1}/button.h

LIBRARY2 = $${ARDUINO_LOCAL_LIBRARY_PATH}/TextMenu/src
INCLUDEPATH += $${LIBRARY2}
SOURCES += $${LIBRARY2}/menu.cpp
HEADERS += $${LIBRARY2}/menu.h


FORMS    += mainwindow.ui

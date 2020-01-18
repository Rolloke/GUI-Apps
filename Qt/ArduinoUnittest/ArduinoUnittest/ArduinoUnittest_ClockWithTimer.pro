#-------------------------------------------------
#
# Project created by QtCreator 2019-04-30T17:58:23
#
#-------------------------------------------------

include (ArduinoUnittest.pro)

# Aduino Project Files
ARDUINO_PATH = ../../../Arduino/Sketches/Uhren/ClockWithTimers/clockwithtimers
INCLUDEPATH += $${ARDUINO_PATH}
HEADERS += $${ARDUINO_PATH}/SettingStates.h
HEADERS += $${ARDUINO_PATH}/ioAt328.h
HEADERS += $${ARDUINO_PATH}/ioMega2560.h

SOURCES += clock_with_timer.cpp
SOURCES += $${ARDUINO_PATH}/clockwithtimers.ino
SOURCES += $${ARDUINO_PATH}/SettingStates.cpp

# Arduino Libraries
LIBRARY1 = $${ARDUINO_LOCAL_LIBRARY_PATH}/Button/src
INCLUDEPATH += $${LIBRARY1}
SOURCES += $${LIBRARY1}/button.cpp
HEADERS += $${LIBRARY1}/button.h

LIBRARY2 = $${ARDUINO_LIBRARY_PATH}/Time
INCLUDEPATH += $${LIBRARY2}
SOURCES += $${LIBRARY2}/Time.cpp
HEADERS += $${LIBRARY2}/Time.h
SOURCES += $${LIBRARY2}/DateStrings.cpp

LIBRARY3 = $${ARDUINO_LIBRARY_PATH}/TimeAlarms
INCLUDEPATH += $${LIBRARY3}
SOURCES += $${LIBRARY3}/TimeAlarms.cpp
HEADERS += $${LIBRARY3}/TimeAlarms.h

LIBRARY4 = $${ARDUINO_LIBRARY_PATH}/DS1307RTC
INCLUDEPATH += $${LIBRARY4}
SOURCES += $${LIBRARY4}/DS1307RTC.cpp
HEADERS += $${LIBRARY4}/DS1307RTC.h

LIBRARY5 = $${ARDUINO_LOCAL_LIBRARY_PATH}/Melody/src
INCLUDEPATH += $${LIBRARY5}
SOURCES += $${LIBRARY5}/Melody.cpp
HEADERS += $${LIBRARY5}/Melody.h


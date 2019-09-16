cmake_minimum_required(VERSION 2.8.9)
set(TARGET_NAME ArduinoUnittestCDT)

project(${TARGET_NAME})

add_definitions(-D_USE_MATH_DEFINES)


if(UNIX)
set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++0x )
endif(UNIX)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


#Qt
find_package(Qt5Core)
find_package(Qt5Widgets)
find_package(Qt5Network)
find_package(Qt5OpenGL)


set(CMAKE_AUTOMOC ON)
add_definitions(-DQT_NO_KEYWORDS)



#binaer_analog_uhr.cpp
#configurabledelaytimerino.cpp
#FunctionGeneratorIno.cpp
#OctansSimulatorino.cpp
#SimpleMenu.cpp

#mainwindow.ui

#DISTFILES += $${ARDUINO_PATH}/ConfigurableDelayTimer.ino


set(HEADER_FILES
    ArduinoUnittest/WString.h
    ArduinoUnittest/Wire.h
    ArduinoUnittest/WProgram.h
    ArduinoUnittest/Stream.h
    ArduinoUnittest/unittest.h
    ArduinoUnittest/serialinterface.h
    ArduinoUnittest/Print.h
    ArduinoUnittest/Printable.h
    ArduinoUnittest/mainwindow.h
    ArduinoUnittest/LiquidCrystal.h
    ArduinoUnittest/EEPROM.h
    ArduinoUnittest/Arduino.h
    )

set(SOURCE_FILES
    ArduinoUnittest/EEPROM.cpp
    ArduinoUnittest/LiquidCrystal.cpp
    ArduinoUnittest/main.cpp
    ArduinoUnittest/mainwindow.cpp
    ArduinoUnittest/Print.cpp
    ArduinoUnittest/serialinterface.cpp
    ArduinoUnittest/Stream.cpp
    ArduinoUnittest/Wire.cpp
    ArduinoUnittest/WString.cpp
    ArduinoUnittest/Arduino.cpp
    )

file(GLOB_RECURSE UI_FILES ArduinoUnittest/*.ui)


set(ARDUINO_PATH /home/rolf/Projekte/Arduino/Sketches/ConfigurableDelayTimer)

set(ARDUINO_FILES
    ArduinoUnittest/configurabledelaytimerino.cpp
    ${ARDUINO_PATH}/ConfigurableDelayTimer.ino
    ${ARDUINO_PATH}/Input.h
    ${ARDUINO_PATH}/Input.cpp
    ${ARDUINO_PATH}/Output.h
    ${ARDUINO_PATH}/Output.cpp
    )

set(LIBRARY1 /opt/arduino-1.8.9/libraries/Button/src)

set(LIBRARY2 /opt/arduino-1.8.9/libraries/TextMenu/src)

set(LIBRARY3 /opt/arduino-1.8.9/libraries/TextTools/src)

set(LIBRARY_FILES
    ${LIBRARY1}/button.cpp
    ${LIBRARY1}/button.h
    ${LIBRARY2}/menu.cpp
    ${LIBRARY2}/menu.h
    ${LIBRARY3}/texttools.cpp
    ${LIBRARY3}/texttools.h
    )



include_directories(${CMAKE_CURRENT_BINARY_DIR}
./ArduinoUnittest
${ARDUINO_PATH}
${LIBRARY1}
${LIBRARY2}
${LIBRARY3}
)

link_directories(
	$ENV{GLEW_ROOT}/lib
	$ENV{QTDIR}/lib
)

qt5_wrap_ui(GENERATED_UI_SOURCE_CODE ${UI_FILES})

add_executable(${TARGET_NAME}
    ${HEADER_FILES}
    ${SOURCE_FILES}
    ${LIBRARY_FILES}
	${RC_FILES}
	${GENERATED_UI_SOURCE_CODE}
    ${GENERATED_MOC_SOURCE_FILES}
    ${GENERATED_QRC_SOURCE_FILES}
    ${ARDUINO_FILES}
)


qt5_use_modules(${TARGET_NAME} Core Widgets Network OpenGL)


target_link_libraries(${TARGET_NAME}
    ${QTMAIN_LIBRARIES}
)

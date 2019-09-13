cmake_minimum_required(VERSION 2.8.9)

project(ArduinoUnittest)

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

if(WIN32)
set(QTMAIN_LIBRARIES optimized qtmain debug  qtmaind)
set(RC_FILES resources/syslogger.rc)
endif(WIN32)


set(CMAKE_AUTOMOC ON)
add_definitions(-DQT_NO_KEYWORDS)



file(GLOB_RECURSE HEADER_FILES ArduinoUnittest/*.h)
file(GLOB_RECURSE SOURCE_FILES ArduinoUnittest/*.cpp)
file(GLOB_RECURSE UI_FILES ArduinoUnittest/*.ui)



include_directories(${CMAKE_CURRENT_BINARY_DIR}
./ArduinoUnittest
)

link_directories(
	$ENV{GLEW_ROOT}/lib
	$ENV{QTDIR}/lib
)

qt5_wrap_ui(GENERATED_UI_SOURCE_CODE ${UI_FILES})

add_executable(sysLogger
    WIN32
    ${HEADER_FILES}
    ${SOURCE_FILES}
	${RC_FILES}
	${GENERATED_UI_SOURCE_CODE}
    ${GENERATED_MOC_SOURCE_FILES}
    ${GENERATED_QRC_SOURCE_FILES}
)


qt5_use_modules(sysLogger Core Widgets Network OpenGL)


target_link_libraries(sysLogger
    ${QTMAIN_LIBRARIES}
)

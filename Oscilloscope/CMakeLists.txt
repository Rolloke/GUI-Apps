cmake_minimum_required(VERSION 2.8.9)

project(Oscilloscope)
 

if(UNIX)
set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++0x )
endif(UNIX)



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

set(Qt5Core_DIR "/opt/Qt5.12.11/5.12.11/gcc_64/bin/cmake/Qt5Core")
set(Qt5Widgets_DIR "/opt/Qt5.12.11/5.12.11/gcc_64/bin/cmake/Qt5Widgets")
set(Qt5Xml_DIR "/opt/Qt5.12.11/5.12.11/gcc_64/bin/cmake/Qt5Xml")
set(Qt5Gui_DIR "/opt/Qt5.12.11/5.12.11/gcc_64/bin/cmake/Qt5Gui")
set(Qt5Multimedia_DIR "/opt/Qt5.12.11/5.12.11/gcc_64/bin/cmake/Qt5Multimedia")

#find_package(Qt5Core)
#find_package(Qt5Widgets)
#find_package(Qt5Xml)
#find_package(Qt5Gui)
#find_package(Qt5Multimedia)


#if(WIN32)
#set(QTMAIN_LIBRARIES optimized qtmain debug  qtmaind)
set(RC_FILES source/resource/resource.rc)
#endif(WIN32)

set(CMAKE_AUTOMOC ON)
add_definitions(-D_USE_QT_AUDIO)

# define trigger processing:
# 1: processing depending on buffer update flag
# 2: find all trigger points and take the latest trigger point with enough display poinds behind it
#    note: processing 2 is not stable
add_definitions(-DTRIGGER_PROCESSING=1)


file(GLOB_RECURSE HEADER_FILES source/*.h)
file(GLOB_RECURSE SOURCE_FILES source/*.cpp)
file(GLOB_RECURSE UI_FILES source/*.ui)

include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${GENERATED_MOC_SOURCE_FILES}
	${GENERATED_UI_SOURCE_CODE}
        ${RC_FILES}
    ./application_build/x86-release
    ./source
)

#link_directories(
#	$ENV{QTDIR}/lib
#)

qt5_wrap_ui(GENERATED_UI_SOURCE_CODE ${UI_FILES})

add_executable(Oscilloscope
    WIN32
    ${HEADER_FILES}
    ${SOURCE_FILES}
	${RC_FILES}
	${GENERATED_UI_SOURCE_CODE}
    ${GENERATED_MOC_SOURCE_FILES}
    ${GENERATED_QRC_SOURCE_FILES}
)


qt5_use_modules(Oscilloscope Core Widgets Xml Gui Multimedia)

target_link_libraries(Oscilloscope
    ${QTMAIN_LIBRARIES}
)

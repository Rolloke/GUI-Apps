cmake_minimum_required(VERSION 2.8.9)

project(Oscilloscope)
 
add_definitions(-D_USE_MATH_DEFINES)


if(UNIX)
set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++0x )
endif(UNIX)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


#Qt
#CURRENTDIR=$(pwd)

find_package(Qt5Core PATHS /home/rolf/Qt/5.6/gcc_64/lib/cmake)
find_package(Qt5Widgets PATHS /home/rolf/Qt/5.6/gcc_64/lib/cmake)
find_package(Qt5Xml PATHS /home/rolf/Qt/5.6/gcc_64/lib/cmake)
find_package(Qt5Gui PATHS /home/rolf/Qt/5.6/gcc_64/lib/cmake)
find_package(Qt5Multimedia PATHS /home/rolf/Qt/5.6/gcc_64/lib/cmake)

#cd $CURRENTDIR

if(WIN32)
set(QTMAIN_LIBRARIES optimized qtmain debug  qtmaind)
set(RC_FILES resources/Oscilloscope.rc)
endif(WIN32)


set(CMAKE_AUTOMOC ON)
add_definitions(-DQT_NO_KEYWORDS)
add_definitions(-D_USE_QT_AUDIO)

file(GLOB_RECURSE HEADER_FILES source/*.h)
file(GLOB_RECURSE SOURCE_FILES source/*.cpp)
file(GLOB_RECURSE UI_FILES source/*.ui)



include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
	${GENERATED_MOC_SOURCE_FILES}
	${GENERATED_UI_SOURCE_CODE}
    ./application_build/x86-release
    ./source
)

link_directories(
	$ENV{GLEW_ROOT}/lib
	$ENV{QTDIR}/lib
)

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


qt5_use_modules(Oscilloscope Core Widgets Xml Gui)


target_link_libraries(Oscilloscope
    ${QTMAIN_LIBRARIES}
)

cmake_minimum_required(VERSION 2.8.9)
set(PROJECT_NAME acalc)
project(${PROJECT_NAME})

add_definitions(-D_USE_MATH_DEFINES)


if(UNIX)
set (CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -std=c++17 )
endif(UNIX)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


#Qt

if(WIN32)
set(QTMAIN_LIBRARIES optimized qtmain debug  qtmaind)
set(RC_FILES resources/syslogger.rc)
endif(WIN32)


set(CMAKE_AUTOMOC ON)
add_definitions(-DQT_NO_KEYWORDS )



file(GLOB_RECURSE HEADER_FILES src/*.h)
file(GLOB_RECURSE SOURCE_FILES src/*.cpp)
file(GLOB_RECURSE UI_FILES src/*.ui)



include_directories(
	${CMAKE_CURRENT_BINARY_DIR}
)

link_directories(
	$ENV{GLEW_ROOT}/lib
	$ENV{QTDIR}/lib
)

add_executable(${PROJECT_NAME}
    WIN32
    ${HEADER_FILES}
    ${SOURCE_FILES}
)


target_link_libraries(${PROJECT_NAME}
    ${QTMAIN_LIBRARIES}
)

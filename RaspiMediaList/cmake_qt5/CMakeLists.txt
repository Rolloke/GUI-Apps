cmake_minimum_required(VERSION 3.6)

project(RaspiMediaList, VERSION 1.0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(Qt5_SYSTEMWIDE /lib/qt5/bin)
set(Qt5_LOCAL $ENV{HOME}/lib/qt5/bin)
message("Testing against Qt")
if(EXISTS ${Qt5_SYSTEMWIDE})
    message("System-wide Qt found!")
    set(Qt5_BASE_DIR ${Qt5_SYSTEMWIDE})
elseif(EXISTS ${Qt5_LOCAL})
    message("Local Qt found!")
    set(Qt5_BASE_DIR ${Qt5_LOCAL})
else()
    message("No user installed Qt5 found!")
endif()

if(DEFINED Qt5_BASE_DIR)
    message("Adjust CMAKE_PREFIX_PATH")
    set(CMAKE_PREFIX_PATH ${Qt5_BASE_DIR};${CMAKE_PREFIX_PATH})
endif()

find_package(Qt5 REQUIRED COMPONENTS Core)
find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt5 REQUIRED COMPONENTS Gui)
find_package(Qt5 REQUIRED COMPONENTS Xml)
find_package(Qt5 REQUIRED COMPONENTS Multimedia)
find_package(Qt5 REQUIRED COMPONENTS MultimediaWidgets)


set(HEADER 
   ../logger.h
   ../mainwindow.h
   )
set(SOURCE 
   ../mainwindow.ui
   ../logger.cpp
   ../mainwindow.cpp
   ../main.cpp
   )

qt_add_resources(RaspiMediaList RESOURCE
    PREFIX ":/"
    FILES 
    ../resource/resource.qrc
    ../resource/applications-multimedia.png 
    ../resource/applications-multimedia16.png
    ../preferences-desktop-screensaver.png
    ../resource/video-x-generic.png
    ../applications-multimedia.ico
    ../applications-multimedia16.ico
)

add_executable(RaspiMediaList
    ${HEADER}
    ${SOURCE}   
)

target_link_libraries(RaspiMediaList PRIVATE Qt5::Core)
target_link_libraries(RaspiMediaList PRIVATE Qt5::Widgets)
target_link_libraries(RaspiMediaList PRIVATE Qt5::Gui)
target_link_libraries(RaspiMediaList PRIVATE Qt5::Xml)
target_link_libraries(RaspiMediaList PRIVATE Qt5::Multimedia)
target_link_libraries(RaspiMediaList PRIVATE Qt5::MultimediaWidgets)


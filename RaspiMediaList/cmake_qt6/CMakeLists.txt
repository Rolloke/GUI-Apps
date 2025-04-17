cmake_minimum_required(VERSION 3.16)

project(RaspiMediaList, VERSION 1.0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS Xml)
find_package(Qt6 REQUIRED COMPONENTS Multimedia)
find_package(Qt6 REQUIRED COMPONENTS MultimediaWidgets)

qt_standard_project_setup(I18N_TRANSLATED_LANGUAGES de en)

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

qt_add_executable(RaspiMediaList
    ${HEADER}
    ${SOURCE}   
)

target_link_libraries(RaspiMediaList PRIVATE Qt6::Core)
target_link_libraries(RaspiMediaList PRIVATE Qt6::Widgets)
target_link_libraries(RaspiMediaList PRIVATE Qt6::Gui)
target_link_libraries(RaspiMediaList PRIVATE Qt6::Xml)
target_link_libraries(RaspiMediaList PRIVATE Qt6::Multimedia)
target_link_libraries(RaspiMediaList PRIVATE Qt6::MultimediaWidgets)


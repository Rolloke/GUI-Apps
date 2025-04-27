cmake_minimum_required(VERSION 3.16)

project(RecentFilesList, VERSION 1.0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
find_package(Qt6 REQUIRED)

find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Gui)
find_package(Qt6 REQUIRED COMPONENTS Xml)


set(HEADER 
   ../mainwindow.h
   )
set(SOURCE 
   ../mainwindow.ui
   ../mainwindow.cpp
   ../main.cpp
   )

qt_add_executable(RecentFilesList
    ${HEADER}
    ${SOURCE}   
)

target_link_libraries(RecentFilesList PRIVATE Qt6::Core)
target_link_libraries(RecentFilesList PRIVATE Qt6::Widgets)
target_link_libraries(RecentFilesList PRIVATE Qt6::Gui)
target_link_libraries(RecentFilesList PRIVATE Qt6::Xml)



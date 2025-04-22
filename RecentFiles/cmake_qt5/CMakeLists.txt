cmake_minimum_required(VERSION 3.6)

project(RecentFileseList, VERSION 1.0.0.2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

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


set(HEADER 
   ../mainwindow.h
   )
set(SOURCE 
   ../mainwindow.ui
   ../mainwindow.cpp
   ../main.cpp
   )

add_executable(RecentFileseList
    ${HEADER}
    ${SOURCE}
)


target_link_libraries(RecentFileseList PRIVATE Qt5::Core)
target_link_libraries(RecentFileseList PRIVATE Qt5::Widgets)
target_link_libraries(RecentFileseList PRIVATE Qt5::Gui)
target_link_libraries(RecentFileseList PRIVATE Qt5::Xml)


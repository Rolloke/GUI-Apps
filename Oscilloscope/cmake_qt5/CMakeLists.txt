cmake_minimum_required(VERSION 3.6)

project(Oscilloscope, VERSION 1.0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

add_compile_definitions(-D_USE_QT_AUDIO=1)

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
   ../source/aboutdlg.h
   ../source/functiongenerator.h      
   ../source/qdoublelablewithunit.h
   ../source/audiosettings.h    
   ../source/functions.h              
   ../source/qgroupeditwithunit.h
   ../source/Average.h          
   ../source/generatefunction.h       
   ../source/qradiobuttongroup.h
   ../source/calibration.h      
   ../source/logger.h                 
   ../source/qwidgetitemdelegate.h
   ../source/Channel.h          
   ../source/main.h                   
   ../source/scopesettings.h
   ../source/circlevector.h     
   ../source/mainwindow.h             
   ../source/scopeview.h
   ../source/combinecurves.h    
   ../source/matrix.h         
   ../source/settings.h
   ../source/cursors.h          
   ../source/memorypool.h             
   ../source/simd.h
   ../source/data_exchange.h    
   ../source/PolyLineItem.h           
   ../source/sse_helper.h
   ../source/DirectIoDevice.h   
   ../source/pseudorandomgenerator.h  
   ../source/TriggerParameter.h
   ../source/events.h           
   ../source/qaudiooutstream.h        
   ../source/unitprefix.h
   ../source/FFT.h              
   ../source/qaudioparameter.h        
   ../source/workerthreadconnector.h
   ../source/fft_view.h         
   ../source/qaudiorecord.h           
   ../source/xml_functions.h
   ../source/filterfunctions.h  
   ../source/qcomboboxdelegate.h
)

set(SOURCE 
   ../source/aboutdlg.cpp
   ../source/generatefunction.cpp       
   ../source/qgroupeditwithunit.cpp
   ../source/audiosettings.cpp      
   ../source/logger.cpp                 
   ../source/qradiobuttongroup.cpp
   ../source/calibration.cpp        
   ../source/main.cpp                   
   ../source/qwidgetitemdelegate.cpp
   ../source/Channel.cpp            
   ../source/mainwindow.cpp             
   ../source/scopesettings.cpp
   ../source/combinecurves.cpp
   ../source/memorypool.cpp             
   ../source/scopeview.cpp
   ../source/cursors.cpp            
   ../source/PolyLineItem.cpp           
   ../source/settings.cpp
   ../source/DirectIoDevice.cpp     
   ../source/pseudorandomgenerator.cpp  
   ../source/simd.cpp
   ../source/events.cpp             
   ../source/qaudiooutstream.cpp        
   ../source/sse_helper.cpp
   ../source/FFT.cpp                
   ../source/qaudioparameter.cpp        
   ../source/TriggerParameter.cpp
   ../source/fft_view.cpp           
   ../source/qaudiorecord.cpp           
   ../source/unitprefix.cpp
   ../source/filterfunctions.cpp    
   ../source/qcomboboxdelegate.cpp      
   ../source/workerthreadconnector.cpp
   ../source/functiongenerator.cpp  
   ../source/qdoublelablewithunit.cpp   
   ../source/xml_functions.cpp   
)

set(UIFILES
   ../source/aboutdlg.ui
   ../source/calibration.ui        
   ../source/mainwindow.ui
   ../source/audiosettings.ui  
   ../source/functiongenerator.ui  
   ../source/scopesettings.ui
) 

qt_add_resources(Oscilloscope RESOURCE
    PREFIX ":/"
    FILES 
    ../source/resource.qrc
    ../source/resource/applications-multimedia.png 
    ../source/resource/applications-multimedia16.png
    ../source/preferences-desktop-screensaver.png
    ../source/resource/video-x-generic.png
    ../source/applications-multimedia.ico
    ../source/applications-multimedia16.ico
)

add_executable(Oscilloscope
    ${HEADER}
    ${SOURCE}   
    ${UIFILES}   
)

target_include_directories(Oscilloscope PUBLIC
  ../source
)

target_link_libraries(Oscilloscope PRIVATE Qt5::Core)
target_link_libraries(Oscilloscope PRIVATE Qt5::Widgets)
target_link_libraries(Oscilloscope PRIVATE Qt5::Gui)
target_link_libraries(Oscilloscope PRIVATE Qt5::Xml)
target_link_libraries(Oscilloscope PRIVATE Qt5::Multimedia)
target_link_libraries(Oscilloscope PRIVATE Qt5::MultimediaWidgets)


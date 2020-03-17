CONFIG      += plugin debug_and_release
TARGET      = $$qtLibraryTarget(qt2dviewplugin)
TEMPLATE    = lib

HEADERS     = qt2dviewplugin.h
SOURCES     = qt2dviewplugin.cpp
RESOURCES   = icons.qrc
LIBS        += -L. 

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += designer
} else {
    CONFIG += designer
}

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS    += target

include(qt2dview.pri)

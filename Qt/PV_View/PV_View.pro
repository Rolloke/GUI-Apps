QT += core gui serialport

DEFINES += SERIALBUS=1
QT += serialbus

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
unix:QMAKE_CXXFLAGS += -std=c++17
unix:QMAKE_LFLAGS   = -fuse-ld=gold
unix:QMAKE_CFLAGS_WARN_ON += -Wno-expansion-to-defined
unix:QMAKE_CFLAGS_WARN_ON += -Wno-deprecated-copy
unix:QMAKE_CXXFLAGS_WARN_ON += -Wno-expansion-to-defined
unix:QMAKE_CXXFLAGS_WARN_ON += -Wno-deprecated-copy

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    qprogressbarfloat.cpp \
    yaml_structs.cpp

HEADERS += \
    mainwindow.h \
    qprogressbarfloat.h \
    yaml_structs.h

FORMS += \
    mainwindow.ui

INCLUDEPATH += \
    /home/rolf/testproject/mothbus/include \
    /home/rolf/test_projects/yaml-cpp/include

DISTFILES += \
   resource/registers.yaml \
   resource/huawei-sun2000-dongle-powersensor.yaml

unix:!macx:!symbian: LIBS += -lyaml-cpp -L/home/rolf/test_projects/yaml-cpp/build

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource/resource.qrc

#!/bin/bash

SOURCE=$1
BINARY=GitView
PACKAGE=package
DEB_FILE=$PACKAGE/DEBIAN/control
DESTINATION=$PACKAGE/opt/kesoft/$BINARY

echo "source -> $SOURCE"

rm -rf $PACKAGE

mkdir $PACKAGE

# create the DEBIAN directory and the control file:

mkdir $PACKAGE/DEBIAN

echo "Package: $PACKAGE
Version: 1.2.0.1
Section: custom
Priority: optional
Architecture: all
Essential: no
Installed-Size: 1024
Maintainer: rolf-kary-ehlers@t-online.de
Description: Viewer for git repositories" > $PACKAGE/DEBIAN/control

# create the DEBIAN directory and the control file:

mkdir -p $DESTINATION
cp $SOURCE/$BINARY $DESTINATION

# QT libraries determined with ldd $SOURCE/$PACKAGE
QT_LIBS_SRC=$( ldd $SOURCE/$BINARY | grep Qt5 | awk '{print $3}' )
QT_LIBS_NAME=$( ldd $SOURCE/$BINARY | awk '{print $1}' )
cp $QT_LIBS_SRC $DESTINATION

QT_LIBS_SRC=(${QT_LIBS_SRC//$'\n'/ })
#echo "${QT_LIBS_SRC[0]}" 
array=(${QT_LIBS_SRC[0]//// })
n=${#array[@]}
QT_SRC=""
for (( i=0; i < $n-2; i++ )) 
do
   QT_SRC="$QT_SRC/${array[i]}"
done
#echo "$QT_SRC"
QT_PLUGINS="$QT_SRC/plugins"

# copy qt.conf with all references
cp ./qt.conf $DESTINATION
# TODO install to ~/.config
#Source: "langs.model.xml"; DestDir: "{app}"; Flags: ignoreversion

# copy qt plugins
mkdir "$DESTINATION/plugins"
cp -r "$QT_PLUGINS/iconengines" "$DESTINATION/plugins"
cp -r "$QT_PLUGINS/imageformats" "$DESTINATION/plugins"
cp -r "$QT_PLUGINS/platforms" "$DESTINATION/plugins"

# TODO copy resources
# QtWebEngineProcess
#Source: "{#QtTranslationsFolder}/qtwebengine_locales/de.pak"; DestDir: "{app}/translations"; Flags: ignoreversion
#Source: "{#QtResourcesFolder}/icudtl.dat"; DestDir: "{app}\resources"; Flags: ignoreversion
#Source: "{#QtResourcesFolder}/qtwebengine_resources_200p.pak"; DestDir: "{app}\resources"; Flags: ignoreversion
#Source: "{#QtResourcesFolder}/qtwebengine_resources_100p.pak"; DestDir: "{app}\resources"; Flags: ignoreversion
#Source: "{#QtResourcesFolder}/qtwebengine_resources.pak"; DestDir: "{app}\resources"; Flags: ignoreversion
#Source: "{#QtResourcesFolder}/qtwebengine_devtools_resources.pak"; DestDir: "{app}\resources"; Flags: ignoreversion
#Source: "{#QtPluginFolder}/platforms/qwindows.dll"; DestDir: "{app}/plugins/platforms"; Flags: ignoreversion


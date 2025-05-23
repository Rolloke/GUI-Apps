#!/bin/bash

SOURCE=$1
BINARY=GitView
PACKAGE=git-view-1.2.0.1
DEB_FILE=$PACKAGE/DEBIAN/control
DESTINATION=$PACKAGE/opt/kesoft/$BINARY

echo "source -> $SOURCE"

rm -rf $PACKAGE

mkdir $PACKAGE

# create the DEBIAN directory and the control file:

mkdir -p $DESTINATION
cp $SOURCE/$BINARY $DESTINATION

# copy QT libraries determined with ldd $SOURCE/$PACKAGE
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
echo "QT source directory: $QT_SRC"

# copy additional QT libraries 
QT_LIB="$QT_SRC/lib"
cp "$QT_LIB/libQt5XcbQpa.so.5" "$DESTINATION"


# copy qt plugins
mkdir "$DESTINATION/plugins"
QT_PLUGINS="$QT_SRC/plugins"
cp -r "$QT_PLUGINS/iconengines" "$DESTINATION/plugins"
cp -r "$QT_PLUGINS/imageformats" "$DESTINATION/plugins"
cp -r "$QT_PLUGINS/platforms" "$DESTINATION/plugins"

# copy QtWebEngineProcess
mkdir "$DESTINATION/libexec"
QT_libexec="$QT_SRC/libexec"
cp "$QT_libexec/QtWebEngineProcess" "$DESTINATION"

QT_RESOURCE="$QT_SRC/resources"
mkdir "$DESTINATION/resources"
cp "$QT_RESOURCE/icudtl.dat" "$DESTINATION/resources"
cp "$QT_RESOURCE/qtwebengine_resources_200p.pak" "$DESTINATION/resources"
cp "$QT_RESOURCE/qtwebengine_resources_100p.pak" "$DESTINATION/resources"
cp "$QT_RESOURCE/qtwebengine_resources.pak" "$DESTINATION/resources"
cp "$QT_RESOURCE/qtwebengine_devtools_resources.pak" "$DESTINATION/resources"

QT_TRANSLATION="$QT_SRC/translations"
mkdir "$DESTINATION/translations"
cp "$QT_TRANSLATION/qtwebengine_locales/de.pak" "$DESTINATION/translations"

# copy qt.conf with all references
cp ./qt.conf $DESTINATION
# TODO install to ~/.config

#Source: "langs.model.xml"; DestDir: "{app}"; Flags: ignoreversion


# create the DEBIAN directory and the control file:
mkdir $PACKAGE/DEBIAN
echo "Package: $PACKAGE
Version: 1.2.0.1
Section: custom
Priority: optional
Architecture: all
Essential: no
Installed-Size: $( du -c -d 0 | grep '\.' |  awk '{print $1}' )
Maintainer: rolf-kary-ehlers@t-online.de
Description: Viewer for git repositories" > $PACKAGE/DEBIAN/control

# TODO activate
#dpkg-deb --build $PACKAGE

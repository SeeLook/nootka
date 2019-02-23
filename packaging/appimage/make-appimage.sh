#! /bin/sh

###################################################################################
# creates directory structure with all stuff (libraries, icons, etc.),            #
# then creates AppImage                                                           #
#                                                                                 #
# Copyright (C) 2011-2019 by Tomasz Bojczuk <seelook@gmail.com>                   #
#                                                                                 #
# Arguments:                                                                      #
# - source directory                                                              #
# - build directory                                                               #
# - qmake executable path                                                         #
# - Nootka version                                                                #
# install linuxdeployqt & appimagetool first - they have to be in $PATH           #
#                                                                                 #
# To correctly generate AppImage set install prefix to '/usr'                     #
# and when using with older Linux system (i.e. Ubuntu Trusty 14.04)               #
# call                                                                            #
# cmake with -DQT_QMAKE_EXECUTABLE=/opt/qtXX/bin/qmake                            #
###################################################################################


SRC_DIR=$1
BIN_DIR=$2
QMAKE=$3
VERSION=$4

printf "\033[01;35mCreating directory AppDir for AppImage of Nootka-$VERSION"
printf "\033[01;00m"
echo "\n"
echo "qmake found in: $QMAKE"

cd $BIN_DIR

if [ "$( whereis linuxdeployqt | grep '\/' )" ]; then
  echo "-- linuxdeployqt was found"
else
  echo "install linuxdeployqt first"
  exit
fi

if [ "$( whereis appimagetool | grep '\/' )" ]; then
  echo "-- appimagetool was found"
else
  echo "install appimagetool first"
  exit
fi


if [ -d AppDir ]; then
  echo "AppDir already exists... deleting"
  rm -r AppDir
fi

mkdir AppDir

#Install to AppDir
make DESTDIR="AppDir/" install

export PATH="$QMAKE:$PATH"

# Qt base translations
TRANS_PATH=$(qmake -query QT_INSTALL_TRANSLATIONS)
cp $TRANS_PATH/qtbase_cs.qm AppDir/usr/share/nootka/lang
cp $TRANS_PATH/qtbase_de.qm AppDir/usr/share/nootka/lang
cp $TRANS_PATH/qtbase_es.qm AppDir/usr/share/nootka/lang
cp $TRANS_PATH/qtbase_fr.qm AppDir/usr/share/nootka/lang
cp $TRANS_PATH/qtbase_hu.qm AppDir/usr/share/nootka/lang
cp $TRANS_PATH/qtbase_pl.qm AppDir/usr/share/nootka/lang
cp $TRANS_PATH/qtbase_ru.qm AppDir/usr/share/nootka/lang

# desktop integration files
cp AppDir/usr/share/nootka/picts/nootka.png AppDir/usr/
cp AppDir/usr/share/applications/nootka.desktop AppDir/usr/

LD_LIBRARY_PATH="$BIN_DIR/AppDir/usr/lib/nootka:$LD_LIBRARY_PATH" linuxdeployqt AppDir/usr/bin/nootka -bundle-non-qt-libs -qmldir=$SRC_DIR/src/qml
# launch it twice to find more libs
LD_LIBRARY_PATH="$BIN_DIR/AppDir/usr/lib/nootka:$LD_LIBRARY_PATH" linuxdeployqt AppDir/usr/bin/nootka -bundle-non-qt-libs -qmldir=$SRC_DIR/src/qml

# qt.conf with translations path pointing inside AppDir and plugins path
cp $SRC_DIR/packaging/appimage/qt.conf AppDir/usr/bin/
# AppRun is a bash script with proper LD_LIBRARY_PATH
cp $SRC_DIR/packaging/nootka AppDir/usr/AppRun

#linuxdeployqt skips xcbglintegrations plugin, so copy it manually
if [ -d AppDir/usr/plugins/xcbglintegrations ]; then
  echo "xcbglintegrations plugin was found"
else
  echo "xcbglintegrations is missing, copying it manually"
  XCB=$(qmake -query QT_INSTALL_PLUGINS)
  cp -r $XCB/xcbglintegrations AppDir/usr/plugins/
fi

# Fix for XCB platform plugin - copy X11 library and remove freetype
cp /usr/lib/x86_64-linux-gnu/libX11.so.6.3.0 AppDir/usr/lib/libX11.so.6
cp /lib/x86_64-linux-gnu/libnsl-2.23.so AppDir/usr/lib/libnsl.so.1
rm AppDir/usr/lib/libfreetype*

# make all contents pointing to itself as a usr/
cd AppDir/usr
ln -s ./. usr
cd ../..

# finally, generate AppImage
appimagetool AppDir/usr nootka-$VERSION-x86_64.AppImage



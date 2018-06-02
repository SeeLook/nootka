#! /bin/sh

###################################################################################
# creates directory structure with all staff (libraries, icons, etc.),            #
# then creates AppImage                                                           #
#                                                                                 #
# Copyright (C) 2011-2018 by Tomasz Bojczuk <seelook@gmail.com>                   #
#                                                                                 #
# Arguments:                                                                      #
# - source directory                                                              #
# - build directory                                                               #
# - Qt directory                                                                  #
# - Nootka version                                                                #
#                                                                                 #
# When older Linux system is used for build (i.e. Ubuntu Trusty 14.04)            #
# and external PPA is used to take Qt version 5.6.2 - it uses own environment     #
# during compilation (that Qt is installed into /opt):                            #
# $source /opt/qt56/bin/set-env.sh                                                #
# To make this script working, just call make appimage from another terminal      #
# but not in the same one as the configure process                                #
###################################################################################


SRC_DIR=$1
BIN_DIR=$2
QT_DIR=$3
VERSION=$4

printf "\033[01;35mCreating directory AppDir for AppImage of Nootka-$VERSION"
printf "\033[01;00m"
echo

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


# Qt base translations
TRANS_PATH=$(qmake -query QT_INSTALL_TRANSLATIONS)
cp $TRANS_PATH/qtbase_cs.qm AppDir/usr/share/nootka/lang
cp $TRANS_PATH/qtbase_de.qm AppDir/usr/share/nootka/lang
cp $TRANS_PATH/qtbase_fr.qm AppDir/usr/share/nootka/lang
cp $TRANS_PATH/qtbase_hu.qm AppDir/usr/share/nootka/lang
cp $TRANS_PATH/qtbase_pl.qm AppDir/usr/share/nootka/lang
cp $TRANS_PATH/qtbase_ru.qm AppDir/usr/share/nootka/lang

# desktop integration files
cp AppDir/usr/share/nootka/picts/nootka.png AppDir/usr/
cp AppDir/usr/share/applications/nootka.desktop AppDir/usr/


LD_LIBRARY_PATH=AppDir/usr/lib/nootka:$LD_LIBRARY_PATH linuxdeployqt AppDir/usr/bin/nootka -bundle-non-qt-libs

# qt.conf with translations path pointing inside AppDir and plugins path
cp $SRC_DIR/packaging/appimage/qt.conf AppDir/usr/bin/
# AppRun is a bash script with proper LD_LIBRARY_PATH
cp $SRC_DIR/packaging/nootka AppDir/usr/AppRun

# move all libraries detected by linuxdeployqt to lib/nootka, LD_LIBRARY_PATH is set to it
mv AppDir/usr/bin/lib/* AppDir/usr/lib/nootka
rm -r AppDir/usr/bin/lib/

# Fix for XCB platform plugin - copy X11 library and remove freetype
cp /usr/lib/x86_64-linux-gnu/libX11.so.6.3.0 AppDir/usr/lib/nootka/libX11.so.6
rm AppDir/usr/lib/nootka/libfreetype*

# make all contents pointing to itself as a usr/
cd AppDir/usr
ln -s ./. usr
cd ../..

# finally, generate AppImage
appimagetool AppDir/usr nootka-$VERSION-x86_64.AppImage



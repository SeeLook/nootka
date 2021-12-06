#! /bin/sh

###################################################################################
# creates directory structure with all stuff (libraries, icons, etc.),            #
# then creates AppImage                                                           #
#                                                                                 #
# Copyright (C) 2011-2021 by Tomasz Bojczuk <seelook@gmail.com>                   #
#                                                                                 #
# Arguments:                                                                      #
# - source directory                                                              #
# - build directory                                                               #
# - qmake executable path                                                         #
# - Nootka version                                                                #
# install linuxdeployqt & appimagetool first - they have to be in $PATH           #
# or they will be downloaded                                                      #
#                                                                                 #
# To correctly generate AppImage set install prefix to '/usr'                     #
# and when using with older Linux system (i.e. Ubuntu 18.04) use:                 #
# -DQT_QMAKE_EXECUTABLE=/opt/qtXX/bin/qmake                                       #
# Also -DWITH_STATIC_LIB=ON is required to generate proper AppImage               #
# so call                                                                         #
# cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr\                   #
# -DWITH_STATIC_LIB=ON -DQT_QMAKE_EXECUTABLE=/opt/qt512/bin/qmake                 #
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
  LIN_DEP_QT=linuxdeployqt
else
  echo "-- fetching linuxdeployqt"
  wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
  chmod a+x linuxdeployqt-continuous-x86_64.AppImage
  LIN_DEP_QT="./linuxdeployqt-continuous-x86_64.AppImage --appimage-extract-and-run"
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
mkdir AppDir/usr/translations
cp $TRANS_PATH/qtbase_cs.qm AppDir/usr/translations
cp $TRANS_PATH/qtbase_de.qm AppDir/usr/translations
cp $TRANS_PATH/qtbase_es.qm AppDir/usr/translations
cp $TRANS_PATH/qtbase_fr.qm AppDir/usr/translations
cp $TRANS_PATH/qtbase_hu.qm AppDir/usr/translations
cp $TRANS_PATH/qtbase_it.qm AppDir/usr/translations
cp $TRANS_PATH/qtbase_pl.qm AppDir/usr/translations
cp $TRANS_PATH/qtbase_ru.qm AppDir/usr/translations
cp $TRANS_PATH/qtbase_uk.qm AppDir/usr/translations

# desktop integration files
mv AppDir/usr/share/metainfo/nootka.appdata.xml AppDir/usr/share/metainfo/net.sf.nootka.appdata.xml
mv AppDir/usr/share/applications/nootka.desktop AppDir/usr/share/applications/net.sf.nootka.desktop
cp AppDir/usr/share/nootka/picts/nootka.png AppDir/
rm -r AppDir/usr/share/icons/hicolor

# Due to AppImage is usually built under Ubuntu/Debian, full license file is not copied by install target
cp $SRC_DIR/LICENSE AppDir/usr/share/nootka/gpl

$LIN_DEP_QT AppDir/usr/share/applications/*.desktop -bundle-non-qt-libs -no-translations -qmldir=$SRC_DIR/src/qml -qmake=$QMAKE -appimage

# Obtain git commits number
BUILD=$(git -C $SRC_DIR rev-list HEAD --count)
mv Nootka*.AppImage Nootka-$VERSION-b$BUILD-x86_64.AppImage




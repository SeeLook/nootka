#! /bin/bash
# This script helps to prepare Mac Os bundle 
# params are:
# install - path_to_nootka_bundle Qt_binary_dir path_to_FFTW_dynlib
# Author:
#           Tomasz Bojczuk <tomaszbojczuk@gmail.com>

#B_PATH=$1/nootka.app/Contents/Resurces
B_PATH=$1/src/nootka.app
# QT_PATH=$(dirname $2)
MACDEPOLOYQT=$2/macdeployqt
FFTW_LIB=$3
VORBFILE_LIB=$4

echo "Preparing bundle to install"

if [ -L $FFTW_LIB ]; then
	printf "\033[01;31m Converting $FFTW_LIB to regular file !!!\033[01;00m\n"
	FF_DIR=$(dirname $FFTW_LIB)
	FF_FILE=$(readlink $FFTW_LIB)
	FFTW_LIB=$(echo "$FF_DIR/$FF_FILE")
	echo $FFTW_LIB
fi
mkdir $B_PATH/Contents/Frameworks
cp $FFTW_LIB $B_PATH/Contents/Frameworks/


OGG_DIR=$(dirname $VORBFILE_LIB)
if [ -L $VORBFILE_LIB ]; then
  printf "\033[01;31m Converting $VORBFILE_LIB to regular file !!!\033[01;00m\n"
  OGG_FILE=$(readlink $VORBFILE_LIB)
  VORBFILE_LIB=$(echo "$OGG_DIR/$OGG_FILE")
  echo $VORBFILE_LIB
fi
cp $VORBFILE_LIB $B_PATH/Contents/Frameworks/

VORBIS_LIB=$(echo "$OGG_DIR/libvorbis.dylib")
if [ -L $VORBIS_LIB ]; then
  printf "\033[01;31m Converting $VORBIS_LIB to regular file !!!\033[01;00m\n"
  VOB_FILE=$(readlink $VORBIS_LIB)
  VORBIS_LIB=$(echo "$OGG_DIR/$VOB_FILE")
  echo $VORBIS_LIB
fi
cp $VORBIS_LIB $B_PATH/Contents/Frameworks/

OGG_LIB=$(echo "$OGG_DIR/libogg.dylib")
if [ -L $OGG_LIB ]; then
  printf "\033[01;31m Converting $OGG_LIB to regular file !!!\033[01;00m\n"
  OG_FILE=$(readlink $OGG_LIB)
  OGG_LIB=$(echo "$OGG_DIR/$OG_FILE")
  echo $OGG_LIB
fi
 cp $OGG_LIB $B_PATH/Contents/Frameworks/
#  exit


if [ -d "$B_PATH/Contents/Resources" ]; then
        echo "Cleaning Resurces Dir"
         rm $B_PATH/Contents/Resources/* # There are only links
else
        echo "no dir $B_PATH"
fi

# if [ -f $QT_PATH/bin/macdeployqt ]; then
if [ -f $MACDEPOLOYQT ]; then
#         $QT_PATH/bin/macdeployqt $B_PATH
    $MACDEPOLOYQT $B_PATH
	echo "deploy qt done"
     if [ -d $B_PATH/Contents/Frameworks/QtSvg.framework ]; then
        rm -r $B_PATH/Contents/Frameworks/QtSvg.framework
        rm -r $B_PATH/Contents/Frameworks/QtDeclarative.framework
        rm -r $B_PATH/Contents/Frameworks/QtMultimedia.framework
        rm -r $B_PATH/Contents/Frameworks/QtScript.framework
        rm -r $B_PATH/Contents/Frameworks/QtSql.framework
        rm -r $B_PATH/Contents/Frameworks/QtXmlPatterns.framework
        rm -r $B_PATH/Contents/Plugins
        rm -r $B_PATH/Contents/Pluglns
    fi
else
	echo "Can not find macdeployqt executable"
fi


#install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore nootka.app/Contents/Frameworks/QtCore.framework/Versions/4/QtCore
#install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui  nootka.app/Contents/Frameworks/QtGui.framework/Versions/4/QtGui
#install_name_tool -id @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork nootka.app/Contents/Frameworks/QtNetwork.framework/Versions/4/QtNetwork

#install_name_tool -change QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore nootka.app/Contents/MacOS/nootka-updater
#install_name_tool -change QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui  nootka.app/Contents/MacOS/nootka-updater
#install_name_tool -change QtNetwork.framework/Versions/4/QtNetwork @executable_path/../Frameworks/QtNetwork.framework/Versions/4/QtNetwork nootka.app/Contents/MacOS/nootka-updater
#hdiutil create -format UDBZ -quiet -srcfolder nootka.app nootka.dmg

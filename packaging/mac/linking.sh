#! /bin/bash

QTLIBS=$1
QTMULTIMEDIA=$2
#QTCORELIB=$2
#QTGUILIB=$3
FFTW=$3
 BUNDLE_PATH=$4/src/nootka.app
# QTLIBS=$6
#BUNDLE_PATH=$4/src/

echo $QTMULTIMEDIA
#echo $QTCORELIB
#echo $QTGUILIB
echo $FFTW
echo $BUNDLE_PATH
echo $QTLIBS



# otool -L QtGui.framework/QtGui
mkdir -p $BUNDLE_PATH/Contents/Frameworks/QtCore.framework/Versions/4/
 cp -R $QTLIBS/QtCore.framework/Versions/4/QtCore $BUNDLE_PATH/Contents/Frameworks/QtCore.framework/Versions/4/
#cp -R $QTCORELIB $BUNDLE_PATH/Contents/Frameworks/
#cp -R $QTGUILIB $BUNDLE_PATH/Contents/Frameworks/
#cp -R $QTMULTIMEDIA $BUNDLE_PATH/Contents/Frameworks/
 mkdir -p $BUNDLE_PATH/Contents/Frameworks/QtGui.framework/Versions/4/
 cp -R $QTLIBS/QtGui.framework/Versions/4/QtGui $BUNDLE_PATH/Contents/Frameworks/QtGui.framework/Versions/4/
 mkdir -p $BUNDLE_PATH/Contents/Frameworks/QtMultimedia.framework/Versions/4/
 cp -R $QTLIBS/QtMultimedia.framework/Versions/4/QtMultimedia $BUNDLE_PATH/Contents/Frameworks/QtMultimedia.framework/Versions/4/
 cp -R $FFTW  $BUNDLE_PATH/Contents/Frameworks




cd $BUNDLE_PATH
 install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore Contents/Frameworks/QtCore.framework/Versions/4/QtCore
  install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui Contents/Frameworks/QtGui.framework/Versions/4/QtGui
  install_name_tool -id @executable_path/../Frameworks/QMultimedia.framework/Versions/4/QtMultimedia Contents/Frameworks/QtMultimedia.framework/Versions/4/QtMultimedia
  install_name_tool -id @executable_path/../Frameworks/libfftw3f.3.dylib Contents/Frameworks/libfftw3f.3.dylib

install_name_tool -change $QTLIBS/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore Contents/MacOs/nootka
  install_name_tool -change $QTLIBS/QtGui.framework/Versions/4/QtGui @executable_path/../Frameworks/QtGui.framework/Versions/4/QtGui Contents/MacOs/nootka
  install_name_tool -change $QTLIBS/QtMultimedia.framework/Versions/4/QtMultimedia @executable_path/../Frameworks/QtMultimedia.framework/Versions/4/QtMultimedia Contents/MacOs/nootka
  install_name_tool -change $FFTW @executable_path/../Frameworks/libfftw3f.3.dylib Contents/MacOs/nootka

 install_name_tool -change $QTLIBS/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore Contents/Frameworks/QtGui.framework/Versions/4/QtGui
 install_name_tool -change $QTLIBS/QtCore.framework/Versions/4/QtCore @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore Contents/Frameworks/QtMultimedia.framework/Versions/4/QtMultimedia

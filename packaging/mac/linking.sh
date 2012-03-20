#! /bin/bash

QTMULTIMEDIA=$1
QTLIBS=$2
FFTW=$3
 BUNDLE_PATH=$4/src/nootka.app
#BUNDLE_PATH=$4/src/

echo $QTMULTIMEDIA
echo $QTLIBS
echo $FFTW
echo $BUNDLE_PATH



# otool -L QtGui.framework/QtGui
 mkdir -p $BUNDLE_PATH/Contents/Frameworks/QtCore.framework/Versions/4/
 cp -R $QTLIBS/QtCore.framework/Versions/4/QtCore $BUNDLE_PATH/Contents/Frameworks/QtCore.framework/Versions/4/
 mkdir -p $BUNDLE_PATH/Contents/Frameworks/QtGui.framework/Versions/4/
 cp -R $QTLIBS/QtGui.framework/Versions/4/QtGui $BUNDLE_PATH/Contents/Frameworks/QtGui.framework/Versions/4/
 mkdir -p $BUNDLE_PATH/Contents/Frameworks/QtMultimedia.framework/Versions/4/
 cp -R $QTLIBS/QtMultimedia.framework/Versions/4/QtMultimedia $BUNDLE_PATH/Contents/Frameworks/QtMultimedia.framework/Versions/4/
 cp -R $FFTW  $BUNDLE_PATH/Contents/Frameworks




cd $BUNDLE_PATH
 install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/4/QtCore nootka.app/Contents/Frameworks/QtCore.framework/Versions/4/QtCore
#  install_name_tool -id @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui
#         nootka.app/Contents/Frameworks/QtGui.framework/Versions/4.0/QtGui

# install_name_tool -change path/to/Qt/lib/QtCore.framework/Versions/4.0/QtCore
#          @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore
#          nootka.app/Contents/MacOs/nootka
#  install_name_tool -change path/to/qt/lib/QtGui.framework/Versions/4.0/QtGui
#          @executable_path/../Frameworks/QtGui.framework/Versions/4.0/QtGui
#          nootka.app/Contents/MacOs/nootka

# install_name_tool -change path/to/Qt/lib/QtCore.framework/Versions/4.0/QtCore
#          @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore
#          nootka.app/Contents/Frameworks/QtGui.framework/Versions/4.0/QtGui

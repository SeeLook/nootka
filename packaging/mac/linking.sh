#! /bin/bash

QTMULTIMEDIA=$1
QTLIBS=$2
FFTW=$3
# BUNDLE_PATH=$4/src/nootka.app
BUNDLE_PATH=$4/src/

echo $QTMULTIMEDIA
echo $QTLIBS
echo $FFTW
echo $BUNDLE_PATH



# otool -L QtGui.framework/QtGui
# mkdir nootka.app/Contents/Frameworks
#  cp -R $QTLIBS/QtCore.framework nootka.app/Contents/Frameworks
#  cp -R $QTLIBS/libQtCore.so $BUNDLE_PATH
#  cp -R $QTLIBS/QtGui.framework  nootka.app/Contents/Frameworks
#  cp -R $QTMULTIMEDIA  nootka.app/Contents/Frameworks
#  cp -R $FFTW  nootka.app/Contents/Frameworks

# install_name_tool -id @executable_path/../Frameworks/QtCore.framework/Versions/4.0/QtCore
#         nootka.app/Contents/Frameworks/QtCore.framework/Versions/4.0/QtCore
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
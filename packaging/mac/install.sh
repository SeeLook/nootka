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

echo "Preparing bundle to install"

if [ -L $FFTW_LIB ]; then
	printf "\033[01;31m $FFTW_LIB is a link but regular file was expected. Run cmake again and give proper file !!!\033[01;00m\n"
	FFTW_LIB=$(realpath $FFTW_LIB)
else
	mkdir $B_PATH/Contents/Frameworks
	cp $FFTW_LIB $B_PATH/Contents/Frameworks/
fi


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
        rm -r $B_PATH/Contents/Frameworks/QtNetwork.framework
        rm -r $B_PATH/Contents/Frameworks/QtScript.framework
        rm -r $B_PATH/Contents/Frameworks/QtSql.framework
        rm -r $B_PATH/Contents/Frameworks/QtXmlPatterns.framework
        rm -r $B_PATH/Contents/Plugins
    fi
else
	echo "Can not find macdeployqt executable"
fi


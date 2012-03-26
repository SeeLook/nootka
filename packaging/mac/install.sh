#! /bin/bash
# This script helps to prepare Mac Os bundle 
# params are:
# install - path_to_nootka_bundle Qt_base_dir
# Author:
#           Tomasz Bojczuk <tomaszbojczuk@gmail.com>

#B_PATH=$1/nootka.app/Contents/Resurces
B_PATH=$1/src/nootka.app
QT_PATH=$(dirname $2)
FFTW_LIB=$3

echo "Preparing bundle to install"

if [ -L $FFTW_LIB ]; then
	printf "\033[01;31m $FFTW_LIB is a link but regular file was expected. Run cmake again and give proper file !!!\033[01;00m\n"
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

if [ -f $QT_PATH/bin/macdeployqt ]; then
        $QT_PATH/bin/macdeployqt $B_PATH
	echo "deploy qt done"
# 	rm -r $B_PATH/Context/Frameworks/QtSvg.framework
else
	echo "Can not find macdeployqt executable"
fi


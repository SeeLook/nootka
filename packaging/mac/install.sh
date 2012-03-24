#! /bin/bash
# This script helps to prepare Mac Os bundle 
# params are:
# install - path_to_nootka_bundle Qt_base_dir
# Author:
#           Tomasz Bojczuk <tomaszbojczuk@gmail.com>

#B_PATH=$1/nootka.app/Contents/Resurces
B_PATH=$1/src/nootka.app
QT_PATH=$(dirname $2)

echo "path: $B_PATH"
echo "path: $QT_PATH"

if [ -f "B_PATH" ]; then
    echo "i have nootka"
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


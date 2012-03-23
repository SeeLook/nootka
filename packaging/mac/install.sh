#! /bin/bash
# This script helps to prepare Mac Os bundle 
# params are:
# install - path_to_nootka_bundle Qt_base_dir
# Author:
#           Tomasz Bojczuk <tomaszbojczuk@gmail.com>

B_PATH=$1
QT= dirname $2

if [ -d $B_PATH/Context/Resurces ]; then
	echo "Cleaning Resurces Dir"
# 	rm $B_PATH/Context/Resurces/* # There are only links

fi

if [ -f $QT/bin/macdeployqt ]; then
# 	$QT/bin/macdeployqt $B_PATH
	echo "deploy qt done"
# 	rm -r $B_PATH/Context/Frameworks/QtSvg.framework
else
	echo "Can not find macdeployqt executable"
fi


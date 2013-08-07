#! /bin/bash
#
# This script creates links necessary to run Nootka without installation
# Params are:
# SRC_DIR - source dir
# DST_DIR - destination dir

SRC_DIR=$1
DST_DIR=$2

printf "\033[01;35mLinking source files necessary to run Nootka" 
printf "\033[01;00m"
echo

echo $DST_DIR

if [ $( echo $DST_DIR| grep 'nootka.app')  ]; then
	echo "Nac os"
else
	echo "Linux"
	if [ -d $DST_DIR/share ]; then
		echo "Already done"
		exit
	else
		mkdir -p $DST_DIR/share/nootka
		ln -s $SRC_DIR/fonts $DST_DIR/share/nootka/
		ln -s $SRC_DIR/changes $DST_DIR/share/nootka/
		ln -s $SRC_DIR/gpl $DST_DIR/share/nootka/
		ln -s $SRC_DIR/picts $DST_DIR/share/nootka/
		ln -s $SRC_DIR/lang $DST_DIR/share/nootka/
		ln -s $SRC_DIR/sounds $DST_DIR/share/nootka/
	fi
fi

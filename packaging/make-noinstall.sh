#! /bin/bash
#################################################################################
# This script creates links necessary to run Nootka without installation (in directory where it is built)
# Params are:
# SRC_DIR - source dir
# DST_DIR - destination dir (Linux: nootka/../share/nootka | Mac nootka/../Resources)
#################################################################################

SRC_DIR=$1
DST_DIR=$2

printf "\033[01;35mLinking source files necessary to run Nootka" 
printf "\033[01;00m"
echo

#echo $DST_DIR

if [ $( echo $DST_DIR| grep 'Resources')  ]; then
  echo "Linking for Mac Os"
  DST_DIR=$2/
else
  echo "Linking for Linux"
  DST_DIR=$2/share/nootka/
fi

echo $DST_DIR
if [ -d $DST_DIR ]; then
  echo "Already done... Exiting!"
  exit
else
  mkdir -p $DST_DIR
  ln -s $SRC_DIR/fonts $DST_DIR
  ln -s $SRC_DIR/changes $DST_DIR
  ln -s $SRC_DIR/gpl $DST_DIR
  ln -s $SRC_DIR/picts $DST_DIR
  ln -s $SRC_DIR/lang $DST_DIR
  ln -s $SRC_DIR/sounds $DST_DIR
  ln -s $SRC_DIR/levels $DST_DIR
fi


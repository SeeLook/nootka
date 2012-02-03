#! /bin/bash
# Script for building Nootka source tar-ball package
# It usualy is invoked by make src
# USAGE:
# make-src.sh version build-directory source-directory
# 
# Author:
#           Tomasz Bojczuk <tomaszbojczuk@gmail.com>
#

VERSION=$1
BUILD_DIR=$2
SRC_DIR=$3
DST_DIR=$BUILD_DIR/nootka-$VERSION-sources

printf "\033[01;35mGenerating source package for \033[01;32mnootka""-""$VERSION"-"sources" 
printf "\033[01;00m"
echo

mkdir $DST_DIR

cp -r $SRC_DIR/fonts $DST_DIR
cp -r $SRC_DIR/lang $DST_DIR
cp -r $SRC_DIR/mime $DST_DIR
cp -r $SRC_DIR/packaging $DST_DIR
cp -r $SRC_DIR/picts $DST_DIR
mkdir $DST_DIR/sounds
cp $SRC_DIR/sounds/classical-guitar.wav $DST_DIR/sounds/
cp -r $SRC_DIR/src $DST_DIR
cp $SRC_DIR/changelog $DST_DIR
cp $SRC_DIR/copyright $DST_DIR
cp $SRC_DIR/CMakeLists.txt $DST_DIR
cp $SRC_DIR/gpl $DST_DIR
cp $SRC_DIR/README $DST_DIR
cp $SRC_DIR/TODO $DST_DIR

tar -cvjf $DST_DIR.tar.bz2 $DST_DIR

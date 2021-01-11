#! /bin/bash
# Script for building Nootka source tar-ball package
# It usually is invoked by make src
# USAGE:
# make-src.sh version build-directory source-directory
# 
# Author:
#           Tomasz Bojczuk <seelook@gmail.com>
#

VERSION=$1
BUILD_DIR=$2
SRC_DIR=$3
DST_DIR=nootka-$VERSION-source

printf "\033[01;35mGenerating source package for \033[01;32m$DST_DIR" 
printf "\033[01;00m"
echo

cd $BUILD_DIR
mkdir $DST_DIR

cp -r $SRC_DIR/fonts $DST_DIR
cp -r $SRC_DIR/lang $DST_DIR
cp -r $SRC_DIR/levels $DST_DIR
cp -r $SRC_DIR/mime $DST_DIR
cp -r $SRC_DIR/packaging $DST_DIR
cp -r $SRC_DIR/picts $DST_DIR
mkdir $DST_DIR/sounds

cp $SRC_DIR/sounds/*.ogg $DST_DIR/sounds/
cp $SRC_DIR/sounds/*.raw48-16 $DST_DIR/sounds/
cp -r $SRC_DIR/src $DST_DIR
cp $SRC_DIR/changes $DST_DIR
cp $SRC_DIR/copyright $DST_DIR
cp $SRC_DIR/CMakeLists.txt $DST_DIR
cp $SRC_DIR/gpl $DST_DIR
cp $SRC_DIR/README.md $DST_DIR
cp $SRC_DIR/TODO.md $DST_DIR
cp $SRC_DIR/cmake_uninstall.cmake.in $DST_DIR

# TODO git commits number here (Mac needs it only)

tar -cjf $DST_DIR.tar.bz2 $DST_DIR

rm -r $DST_DIR

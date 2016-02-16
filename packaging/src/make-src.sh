#! /bin/bash
# Script for building Nootka source tar-ball package
# It usualy is invoked by make src
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
cp -r $SRC_DIR/mime $DST_DIR
cp -r $SRC_DIR/packaging $DST_DIR
cp -r $SRC_DIR/picts $DST_DIR
mkdir $DST_DIR/sounds
mkdir -p $DST_DIR/.hg/cache
cp $SRC_DIR/sounds/*-guitar.ogg $DST_DIR/sounds/
cp -r $SRC_DIR/src $DST_DIR
cp $SRC_DIR/changes $DST_DIR
cp $SRC_DIR/copyright $DST_DIR
cp $SRC_DIR/CMakeLists.txt $DST_DIR
cp $SRC_DIR/gpl $DST_DIR
cp $SRC_DIR/README $DST_DIR
cp $SRC_DIR/TODO $DST_DIR
cp $SRC_DIR/cmake_uninstall.cmake.in $DST_DIR
if [ -f $SRC_DIR/.hg/cache/branchheads ]; then
		cp $SRC_DIR/.hg/cache/branchheads $DST_DIR/.hg/cache/
fi
if [ -f $SRC_DIR/.hg/cache/branchheads-served ]; then
		cp $SRC_DIR/.hg/cache/branchheads-served $DST_DIR/.hg/cache/branchheads
fi

tar -cjf $DST_DIR.tar.bz2 $DST_DIR

rm -r $DST_DIR

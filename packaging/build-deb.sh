#! /bin/bash
# Script for building Nootka debian biniary package
# It ussualy is invoked by make deb
# USAGE:
# build-deb.sh version build-directory source-directory
# 
# Author:
#           Tomasz Bojczuk <tomaszbojczuk@gmail.com>
#

VERSION=$1
BUILD_DIR=$2
SRC_DIR=$3
ARCH=$(uname -m)

printf "\033[01;35mBuilding Debian binary package for \033[01;32mnootka-$VERSION-$ARCH\n" 
printf "\033[01;00m"

# echo 
# DEB_TOOLS=0
# if [ $(whereis fakeroot|grep bin) ]; then


if [ -f $BUILD_DIR/src/nootka ]; then
  # Cleaning previous 
  if [ -d $BUILD_DIR/debian ]; then
    echo "cleaning...."
    rm -rf $BUILD_DIR/debian
  fi
  # Building dir tree
  mkdir -p $BUILD_DIR/debian/DEBIAN
  mkdir -p $BUILD_DIR/debian/usr/bin
  mkdir -p $BUILD_DIR/debian/usr/share/nootka/doc/nootka





else
  printf "\033[01;31mNootka executable doesn't exist. Compile it first !!!\033[01;00m\n"
fi
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

mkdir -p 
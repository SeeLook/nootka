#! /bin/bash
# Script for building Nootka rpm biniary package
# It usualy is invoked by make rpm
# USAGE:
# build-rpm.sh version build-directory source-directory package-directory
# 
# Author:
#           Tomasz Bojczuk <tomaszbojczuk@gmail.com>
#

VERSION=$1
BUILD_DIR=$2
SRC_DIR=$3
ARCH=$(rpm --eval %_target_cpu) 
PACK_DIR=$4

printf "\033[01;35mBuilding Rpm binary package for \033[01;32mnootka-$VERSION-$ARCH\n" 
printf "\033[01;00m"


echo " - Searching for tools to build rpm package..."
if [ "$(whereis rpm|grep bin)" != "" ]; then
	printf "\trpm found\n"
else
	printf "\trpm not found\n"
	printf "\033[01;31mInstall rpm first !!!\033[01;00m\n"
fi

mkdir -p $BUILD_DIR/rpm
$SRC_DIR/packaging/rpm/spec.sh $VERSION $ARCH > $BUILD_DIR/rpm/nootka.spec
echo "%description:" >> $BUILD_DIR/rpm/nootka.spec
cat $SRC_DIR/packaging/nootka-desc >> $BUILD_DIR/rpm/nootka.spec
printf "\n\n" >> $BUILD_DIR/rpm/nootka.spec
echo "%changelog:" >> $BUILD_DIR/rpm/nootka.spec
$SRC_DIR/packaging/getLastChangesSet.sh $SRC_DIR/changelog >> $BUILD_DIR/rpm/nootka.spec
		

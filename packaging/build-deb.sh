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
ARCH=$(dpkg-architecture -qDEB_BUILD_ARCH)

printf "\033[01;35mBuilding Debian binary package for \033[01;32mnootka-$VERSION-$ARCH\n" 
printf "\033[01;00m"

echo " - Searching for tools for building deb package..."
	DEB_TOOLS=1
	if [ "$(whereis fakeroot|grep bin)" != "" ]; then
		printf "\tfakeroot found\n"
	else
		printf "\tfakeroot not found\n"
		DEB_TOOLS=0
	fi
	if [ "$(whereis dpkg-deb|grep bin)" != "" ]; then
		printf "\tdpkg-deb found\n"
	else
		printf "\tdpkg-deb not found\n"
		DEB_TOOLS=0
	fi
	if [ $DEB_TOOLS == 0 ]; then
	printf "\033[01;31mInstall build-esentiall and fakeroot first !!!\033[01;00m\n"
	fi

#############################################################################################
####  COPYING FILES              ############################################################
#############################################################################################
if [ -f $BUILD_DIR/src/nootka ]; then
	# Cleaning previous 
	if [ -d $BUILD_DIR/debian ]; then
		echo " - cleaning..."
		rm -rf $BUILD_DIR/debian
	fi
	# Building dir tree
	mkdir -p $BUILD_DIR/debian/DEBIAN
	mkdir -p $BUILD_DIR/debian/usr/bin
	mkdir -p $BUILD_DIR/debian/usr/share/nootka/doc/nootka
	mkdir -p $BUILD_DIR/debian/usr/share/nootka/mime/packages
	mkdir -p $BUILD_DIR/debian/usr/share/man/man1

	echo " - copying files..."
	cp $BUILD_DIR/src/nootka $BUILD_DIR/debian/usr/bin/
	$SRC_DIR/packaging/debian/control.sh $VERSION $ARCH > $BUILD_DIR/debian/DEBIAN/control
	cp $SRC_DIR/packaging/nootka.1 $BUILD_DIR/debian/usr/share/man/man1/
	gzip --best $BUILD_DIR/debian/usr/share/man/man1/nootka.1




else
  printf "\033[01;31mNootka executable doesn't exist. Compile it first !!!\033[01;00m\n"
fi
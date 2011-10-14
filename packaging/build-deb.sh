#! /bin/bash
# Script for building Nootka debian biniary package
# It usualy is invoked by make deb
# USAGE:
# build-deb.sh version build-directory source-directory package-directory
# 
# Author:
#           Tomasz Bojczuk <tomaszbojczuk@gmail.com>
#

VERSION=$1
BUILD_DIR=$2
SRC_DIR=$3
PACK_DIR=$4
ARCH=$(dpkg-architecture -qDEB_BUILD_ARCH)


printf "\033[01;35mBuilding Debian binary package for \033[01;32mnootka""_""$VERSION""_""$ARCH\n" 
printf "\033[01;00m"

echo " - Searching for tools to build deb package..."
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
	if [ -d $PACK_DIR ]; then
	    rm -rf $PACK_DIR
	fi
      # Building dir tree
	$SRC_DIR/packaging/make-build-tree.sh $PACK_DIR $SRC_DIR $BUILD_DIR
  # DEBIAN files
	mkdir -p $PACK_DIR/DEBIAN
	$SRC_DIR/packaging/debian/control.sh $VERSION $ARCH > $PACK_DIR/DEBIAN/control
	cp $SRC_DIR/packaging/debian/postinst $PACK_DIR/DEBIAN/
	cp $SRC_DIR/packaging/debian/postrm $PACK_DIR/DEBIAN/

	cp $SRC_DIR/copyright $PACK_DIR/usr/share/doc/nootka/
	$SRC_DIR/packaging/make-chlog.sh $VERSION $PACK_DIR/usr/share/doc/nootka/changelog $SRC_DIR

	echo " - crearting deb package..."
	fakeroot dpkg-deb --build $PACK_DIR

	mv $BUILD_DIR/debian.deb $BUILD_DIR/nootka"_"$VERSION"_"$ARCH.deb

	rm -rf $PACK_DIR


else
  printf "\033[01;31mNootka executable doesn't exist. Compile it first !!!\033[01;00m\n"
fi
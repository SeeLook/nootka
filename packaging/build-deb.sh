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

printf "\033[01;35mBuilding Debian binary package for \033[01;32mnootka-$VERSION""_""$ARCH\n" 
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
	    rm -rf $BUILD_DIR/debian
	fi
      # Building dir tree
	mkdir -p $BUILD_DIR/debian/DEBIAN
	mkdir -p $BUILD_DIR/debian/usr/bin
	mkdir -p $BUILD_DIR/debian/usr/share/doc/nootka
	mkdir -p $BUILD_DIR/debian/usr/share/mime/packages
	mkdir -p $BUILD_DIR/debian/usr/share/man/man1
	mkdir -p $BUILD_DIR/debian/usr/share/applications
	mkdir -p $BUILD_DIR/debian/usr/share/pixmaps
	mkdir -p $BUILD_DIR/debian/usr/share/nootka/lang
	mkdir -p $BUILD_DIR/debian/usr/share/nootka/sounds

	echo " - copying files..."
	cp $BUILD_DIR/src/nootka $BUILD_DIR/debian/usr/bin/
	# DEBIAN files
	$SRC_DIR/packaging/debian/control.sh $VERSION $ARCH > $BUILD_DIR/debian/DEBIAN/control
	cp $SRC_DIR/packaging/debian/postinst $BUILD_DIR/debian/DEBIAN/
	cp $SRC_DIR/packaging/debian/postrm $BUILD_DIR/debian/DEBIAN/

	cp $SRC_DIR/copyright $BUILD_DIR/debian/usr/share/doc/nootka/
	printf "Nootka ($VERSION) all-deb; urgency=low\n\n  * " > $BUILD_DIR/debian/usr/share/doc/nootka/changelog
# 	cat $SRC_DIR/changelog >> $BUILD_DIR/debian/usr/share/doc/nootka/changelog
# 	cat $SRC_DIR/packaging/debian/changelog >> $BUILD_DIR/debian/usr/share/doc/nootka/changelog
# 	echo "" >> $BUILD_DIR/debian/usr/share/doc/nootka/changelog
# 	echo " -- See Look <seelook@gmail.com>  $(date -R)" >> $BUILD_DIR/debian/usr/share/doc/nootka/changelog	
# 	echo "" >> $BUILD_DIR/debian/usr/share/doc/nootka/changelog
	cp $SRC_DIR/packaging/debian/changelog $BUILD_DIR/debian/usr/share/doc/nootka/changelog
	gzip --best $BUILD_DIR/debian/usr/share/doc/nootka/changelog

	cp $SRC_DIR/packaging/nootka.1 $BUILD_DIR/debian/usr/share/man/man1/
	gzip --best $BUILD_DIR/debian/usr/share/man/man1/nootka.1

	$SRC_DIR/mime/nel-noo.sh /usr > $BUILD_DIR/debian/usr/share/mime/packages/nootka.xml
	cp $SRC_DIR/mime/nootka.desktop $BUILD_DIR/debian/usr/share/applications/
	cp $SRC_DIR/picts/nootka.svg $BUILD_DIR/debian/usr/share/pixmaps
	cp $SRC_DIR/picts/levelCreator.png $BUILD_DIR/debian/usr/share/pixmaps
	cp $SRC_DIR/picts/nootka-exam.png $BUILD_DIR/debian/usr/share/pixmaps

	cp $SRC_DIR/LICENSE $BUILD_DIR/debian/usr/share/nootka/
	cp $SRC_DIR/lang/*.qm $BUILD_DIR/debian/usr/share/nootka/lang/
	cp -r $SRC_DIR/fonts $BUILD_DIR/debian/usr/share/nootka/
	cp -r $SRC_DIR/picts $BUILD_DIR/debian/usr/share/nootka/
	cp $SRC_DIR/sounds/classical-guitar.wav $BUILD_DIR/debian/usr/share/nootka/sounds/

	echo " - crearting deb package..."
	fakeroot dpkg-deb --build $BUILD_DIR/debian

	mv debian.deb nootka-$VERSION"_"$ARCH.deb

	rm -rf $BUILD_DIR/debian



else
  printf "\033[01;31mNootka executable doesn't exist. Compile it first !!!\033[01;00m\n"
fi
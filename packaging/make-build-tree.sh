#! /bin/bash
# Script for building Nootka binary files tree
# USAGE:
# make-build-tree.sh root-directory source-directory build-directory
# 
# Author:
#           Tomasz Bojczuk <tomaszbojczuk@gmail.com>
#

ROOT_DIR=$1
SRC_DIR=$2
BUILD_DIR=$3
  # Building dir tree

mkdir -p $ROOT_DIR/usr/bin
mkdir -p $ROOT_DIR/usr/share/doc/nootka
mkdir -p $ROOT_DIR/usr/share/mime/packages
mkdir -p $ROOT_DIR/usr/share/man/man1
mkdir -p $ROOT_DIR/usr/share/applications
mkdir -p $ROOT_DIR/usr/share/pixmaps
mkdir -p $ROOT_DIR/usr/share/nootka/lang
mkdir -p $ROOT_DIR/usr/share/nootka/sounds

echo " - copying files..."
cp $BUILD_DIR/src/nootka $ROOT_DIR/usr/bin/

cp $SRC_DIR/packaging/nootka.1.gz $ROOT_DIR/usr/share/man/man1/

$SRC_DIR/mime/nel-noo.sh /usr > $ROOT_DIR/usr/share/mime/packages/nootka.xml
cp $SRC_DIR/mime/nootka.desktop $ROOT_DIR/usr/share/applications/
cp $SRC_DIR/picts/nootka.svg $ROOT_DIR/usr/share/pixmaps
cp $SRC_DIR/picts/levelCreator.png $ROOT_DIR/usr/share/pixmaps
cp $SRC_DIR/picts/nootka-exam.png $ROOT_DIR/usr/share/pixmaps

cp $SRC_DIR/gpl $ROOT_DIR/usr/share/nootka/
cp $SRC_DIR/lang/*.qm $ROOT_DIR/usr/share/nootka/lang/
cp -r $SRC_DIR/fonts $ROOT_DIR/usr/share/nootka/
cp -r $SRC_DIR/picts $ROOT_DIR/usr/share/nootka/
cp $SRC_DIR/sounds/classical-guitar.wav $ROOT_DIR/usr/share/nootka/sounds/



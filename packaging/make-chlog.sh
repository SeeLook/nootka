#! /bin/bash
# Script for automatical building Nootka changelog file for deb package
# It usualy is invoked by cpack
# USAGE:
# make-chlog.sh version build-directory source-directory
# 
# Author:
#           Tomasz Bojczuk <tomaszbojczuk@gmail.com>
#

VERSION=$1
BUILD_DIR=$2
SRC_DIR=$3

printf "Nootka ($VERSION) all-deb; urgency=low\n" > $BUILD_DIR/doc/changelog
echo "
  * Changes set:
" >> $BUILD_DIR/doc/changelog
$SRC_DIR/packaging/getLastChangesSet.sh $SRC_DIR/changelog >> $BUILD_DIR/doc/changelog
# echo " -- See Look <seelook@gmail.com>  $(date +%G-%m-%d)" >> $BUILD_DIR/doc/changelog
echo "
 -- See Look <seelook@gmail.com>  $(date -R)" >> $BUILD_DIR/doc/changelog
echo "
" >> $BUILD_DIR/doc/changelog
gzip -f --best $BUILD_DIR/doc/changelog
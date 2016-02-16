#! /bin/bash
# Script for automatical building Nootka changelog file for deb package
# It usualy is invoked by cpack
# USAGE:
# make-chlog.sh version dest-file source-directory
# 
# Author:
#           Tomasz Bojczuk <seelook@gmail.com>
#

VERSION=$1
DEST_FILE=$2
SRC_DIR=$3

#This is for RPM
echo "* $(LANG=en date +'%a %b %d %Y') See Look <seelook@gmail.com> " > $DEST_FILE-rpm
$SRC_DIR/packaging/getLastChangesSet.sh $SRC_DIR/changes >> $DEST_FILE-rpm

#This is for DEB
DEST_FILE=$(echo "$DEST_FILE.Debian")
printf "Nootka ($VERSION) all-deb; urgency=low\n" > $DEST_FILE
echo "
  * Changes set:
" >> $DEST_FILE
$SRC_DIR/packaging/getLastChangesSet.sh $SRC_DIR/changes >> $DEST_FILE
# echo " -- See Look <seelook@gmail.com>  $(date +%G-%m-%d)" >> $DEST_FILE
echo "
 -- See Look <seelook@gmail.com>  $(date -R)" >> $DEST_FILE
echo "
" >> $DEST_FILE
gzip -f --best $DEST_FILE
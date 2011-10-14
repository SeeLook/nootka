#! /bin/bash
# Script for automatical building Nootka changelog file for deb package
# It usualy is invoked by cpack
# USAGE:
# make-chlog.sh version dest-file source-directory
# 
# Author:
#           Tomasz Bojczuk <tomaszbojczuk@gmail.com>
#

VERSION=$1
DEST_FILE=$2
SRC_DIR=$3

printf "Nootka ($VERSION) all-deb; urgency=low\n" > $DEST_FILE
echo "
  * Changes set:
" >> $DEST_FILE
$SRC_DIR/packaging/getLastChangesSet.sh $SRC_DIR/changelog >> $DEST_FILE
# echo " -- See Look <seelook@gmail.com>  $(date +%G-%m-%d)" >> $DEST_FILE
echo "
 -- See Look <seelook@gmail.com>  $(date -R)" >> $DEST_FILE
echo "
" >> $DEST_FILE
gzip -f --best $DEST_FILE
#!/bin/sh
# This script calls makeInfoPlist.sh to generate nice info.plist file
# with current version number and buid number taken from hg directory
#

VERSION=$1
SRC=$2
DEST=$3

sh $2/packaging/mac/makeInfoPlist.sh $VERSION $SRC > $DEST/nootka.app/Contents/info.plist

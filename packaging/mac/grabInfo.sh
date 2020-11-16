#!/bin/sh
# This script calls makeInfoPlist.sh to generate nice info.plist file
# with current version number and build number taken from current git commits number
#

VERSION=$1
SRC=$2
DEST=$3

sh $2/packaging/mac/makeInfoPlist.sh $VERSION $SRC > $DEST/nootka.app/Contents/info.plist

#!/bin/bash

# Cretes dmg image
# Takes arguments:
# $1 - version
# $2 - Nootka sources directory path
# $3 - app bundle path

VER=$1
#NOO_SRC=$2
NOO_APP=$3
BUILD=$(git -C $2 rev-list HEAD --count)

hdiutil create -fs HFS+ -srcfolder $NOO_APP/nootka.app -volname "Nootka-$VER-b$BUILD" $NOO_APP/Nootka-$VER-b$BUILD.dmg




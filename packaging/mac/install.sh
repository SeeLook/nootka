#! /bin/bash
# This script copies 3d party libraries to Mac Os bundle
# It converts links to shared libraries into regular files before copying
# params are:
# 1. install path to nootka bundle (CMAKE_INSTALL_PREFIX)
# 2. soundTouch library
# 3. fftw3f library
# 4. vorbis libray (from this location other vorbis libs are obtained - ogg and vorbisFile)
# Author:
#           Tomasz Bojczuk <seelook@gmail.com>

#B_PATH=$1/nootka.app/Contents/Resurces
B_PATH=$1/nootka.app
STOUCH_LIB=$2
FFTW_LIB=$3
VORBFILE_LIB=$4


echo "Copying libraries to bundle"

if [ -L $FFTW_LIB ]; then
	printf "\033[01;31m Converting $FFTW_LIB to regular file !!!\033[01;00m\n"
	FF_DIR=$(dirname $FFTW_LIB)
	FF_FILE=$(readlink $FFTW_LIB)
	FFTW_LIB=$(echo "$FF_DIR/$FF_FILE")
	echo $FFTW_LIB
fi
mkdir $B_PATH/Contents/Frameworks
cp $FFTW_LIB $B_PATH/Contents/Frameworks/

if [ -L $STOUCH_LIB ]; then
  printf "\033[01;31m Converting $STOUCH_LIB to regular file !!!\033[01;00m\n"
  ST_DIR=$(dirname $STOUCH_LIB)
  ST_FILE=$(readlink $STOUCH_LIB)
  STOUCH_LIB=$(echo "$ST_DIR/$ST_FILE")
  echo $STOUCH_LIB
fi
cp $STOUCH_LIB $B_PATH/Contents/Frameworks/

OGG_DIR=$(dirname $VORBFILE_LIB)
if [ -L $VORBFILE_LIB ]; then
  printf "\033[01;31m Converting $VORBFILE_LIB to regular file !!!\033[01;00m\n"
  OGG_FILE=$(readlink $VORBFILE_LIB)
  VORBFILE_LIB=$(echo "$OGG_DIR/$OGG_FILE")
  echo $VORBFILE_LIB
fi
cp $VORBFILE_LIB $B_PATH/Contents/Frameworks/

VORBIS_LIB=$(echo "$OGG_DIR/libvorbis.dylib")
if [ -L $VORBIS_LIB ]; then
  printf "\033[01;31m Converting $VORBIS_LIB to regular file !!!\033[01;00m\n"
  VOB_FILE=$(readlink $VORBIS_LIB)
  VORBIS_LIB=$(echo "$OGG_DIR/$VOB_FILE")
  echo $VORBIS_LIB
fi
cp $VORBIS_LIB $B_PATH/Contents/Frameworks/

OGG_LIB=$(echo "$OGG_DIR/libogg.dylib")
if [ -L $OGG_LIB ]; then
  printf "\033[01;31m Converting $OGG_LIB to regular file !!!\033[01;00m\n"
  OG_FILE=$(readlink $OGG_LIB)
  OGG_LIB=$(echo "$OGG_DIR/$OG_FILE")
  echo $OGG_LIB
fi
 cp $OGG_LIB $B_PATH/Contents/Frameworks/



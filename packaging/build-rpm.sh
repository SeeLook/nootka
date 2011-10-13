#! /bin/bash
# Script for building Nootka debian biniary package
# It usualy is invoked by make deb
# USAGE:
# build-rpm.sh version build-directory source-directory
# 
# Author:
#           Tomasz Bojczuk <tomaszbojczuk@gmail.com>
#

VERSION=$1
BUILD_DIR=$2
SRC_DIR=$3
ARCH=$(rpm --eval %_target_cpu) 

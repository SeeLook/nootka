#!/bin/sh
# USAGE:
#       control.sh nootka-version sys-arch

cat <<EOF
Package: nootka
Priority: optional
Section: Education
Maintainer: See Look <seelook@gmail.com>
Version: $1
Architecture: $2
Depends: libc6 (>= 2.9), libqtgui4, libportaudio2
Bugs: mailto:seelook@gmail.com
Recommends: timidity
Description: Application to help with learning classical score notation.
 Mostly it is for guitarists,
 but it can be used for ear training as well.
 .
 Features:
 numerous settings to personalize for user needs and capabilities,
 natural, classical guitar sound output and also midi
 support for different name-callings of the notes,
 scordatures, exams with different levels,
 levels creator with wide range of settings
 .
 http://nootka.sf.net
EOF

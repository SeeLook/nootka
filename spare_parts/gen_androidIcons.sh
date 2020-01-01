#! /bin/bash


################################################################################
# Generates Nootka mimetype icons from svg to picts/hicolor/ source location
# Obligatory call it from spare_parts location in the sources (exactly where it is)
################################################################################


inkscape -z scalable/examicon-flat.svg -w 36 -e ../src/android/res/drawable-ldpi/exam.png
inkscape -z scalable/examicon.svg -w 48 -e ../src/android/res/drawable-mdpi/exam.png
inkscape -z scalable/examicon.svg -w 72 -e ../src/android/res/drawable-hdpi/exam.png
inkscape -z scalable/examicon.svg -w 96 -e ../src/android/res/drawable-xhdpi/exam.png
inkscape -z scalable/examicon.svg -w 144 -e ../src/android/res/drawable-xxhdpi/exam.png
inkscape -z scalable/examicon.svg -w 192 -e ../src/android/res/drawable-xxxhdpi/exam.png

inkscape -z scalable/levelicon-flat.svg -w 36 -e ../src/android/res/drawable-ldpi/level.png
inkscape -z scalable/levelicon.svg -w 48 -e ../src/android/res/drawable-mdpi/level.png
inkscape -z scalable/levelicon.svg -w 72 -e ../src/android/res/drawable-hdpi/level.png
inkscape -z scalable/levelicon.svg -w 96 -e ../src/android/res/drawable-xhdpi/level.png
inkscape -z scalable/levelicon.svg -w 144 -e ../src/android/res/drawable-xxhdpi/level.png
inkscape -z scalable/levelicon.svg -w 192 -e ../src/android/res/drawable-xxxhdpi/level.png

inkscape -z scalable/nootka-flat.svg -w 36 -e ../src/android/res/drawable-ldpi/icon.png
inkscape -z scalable/nootka.svg -w 48 -e ../src/android/res/drawable-mdpi/icon.png
inkscape -z scalable/nootka.svg -w 72 -e ../src/android/res/drawable-hdpi/icon.png
inkscape -z scalable/nootka.svg -w 96 -e ../src/android/res/drawable-xhdpi/icon.png
inkscape -z scalable/nootka.svg -w 144 -e ../src/android/res/drawable-xxhdpi/icon.png
inkscape -z scalable/nootka.svg -w 192 -e ../src/android/res/drawable-xxxhdpi/icon.png

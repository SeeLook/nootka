#! /bin/bash


################################################################################
# Generates Nootka mimetype icons from svg to picts/hicolor/ source location
# Obligatory call it from spare_parts location in the sources (exactly where it is)
################################################################################


inkscape -z scalable/examicon-flat.svg -w 16 -o ../picts/hicolor/16x16/mimetypes/nootkaexam.png
inkscape -z scalable/examicon-flat.svg -w 24 -o ../picts/hicolor/24x24/mimetypes/nootkaexam.png
inkscape -z scalable/examicon-flat.svg -w 32 -o ../picts/hicolor/32x32/mimetypes/nootkaexam.png
inkscape -z scalable/examicon.svg -w 48 -o ../picts/hicolor/48x48/mimetypes/nootkaexam.png
inkscape -z scalable/examicon.svg -w 64 -o ../picts/hicolor/64x64/mimetypes/nootkaexam.png
inkscape -z scalable/examicon.svg -w 128 -o ../picts/hicolor/128x128/mimetypes/nootkaexam.png
# skip 256px size - installer copies it from picts
# inkscape -z scalable/examicon.svg -w 256 -o ../picts/hicolor/256x256/mimetypes/nootkaexam.png
inkscape -z scalable/examicon.svg -w 512 -o ../picts/hicolor/512x512/mimetypes/nootkaexam.png


inkscape -z scalable/levelicon-flat.svg -w 16 -o ../picts/hicolor/16x16/mimetypes/nootkalevel.png
inkscape -z scalable/levelicon-flat.svg -w 24 -o ../picts/hicolor/24x24/mimetypes/nootkalevel.png
inkscape -z scalable/levelicon-flat.svg -w 32 -o ../picts/hicolor/32x32/mimetypes/nootkalevel.png
inkscape -z scalable/levelicon.svg -w 48 -o ../picts/hicolor/48x48/mimetypes/nootkalevel.png
inkscape -z scalable/levelicon.svg -w 64 -o ../picts/hicolor/64x64/mimetypes/nootkalevel.png
inkscape -z scalable/levelicon.svg -w 128 -o ../picts/hicolor/128x128/mimetypes/nootkalevel.png
# skip 256px size - installer copies it from picts
# inkscape -z scalable/levelicon.svg -w 256 -o ../picts/hicolor/256x256/mimetypes/nootkalevel.png
inkscape -z scalable/levelicon.svg -w 512 -o ../picts/hicolor/512x512/mimetypes/nootkalevel.png


inkscape -z scalable/nootka-flat.svg -w 16 -o ../picts/hicolor/16x16/apps/nootka.png
inkscape -z scalable/nootka-flat.svg -w 24 -o ../picts/hicolor/24x24/apps/nootka.png
inkscape -z scalable/nootka-flat.svg -w 32 -o ../picts/hicolor/32x32/apps/nootka.png
inkscape -z scalable/nootka.svg -w 48 -o ../picts/hicolor/48x48/apps/nootka.png
inkscape -z scalable/nootka.svg -w 64 -o ../picts/hicolor/64x64/apps/nootka.png
inkscape -z scalable/nootka.svg -w 128 -o ../picts/hicolor/128x128/apps/nootka.png
inkscape -z scalable/nootka.svg -w 256 -o ../picts/hicolor/256x256/apps/nootka.png
# skip 512px size - installer copies it from picts
#inkscape -z scalable/nootka.svg -w 512 -o ../picts/hicolor/512x512/apps/nootka.png

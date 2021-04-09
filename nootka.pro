#=================================================================
# This file is part of Nootka (http://nootka.sf.net)
# Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)
# on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)
#
# Main Nootka pro
# Nootka consists of core & sound libraries and main executable
#=================================================================

TARGET = nootka
TEMPLATE = subdirs

SUBDIRS = src/libs/core\
          src/libs/sound\
          src\


CONFIG += ordered

src.depends = src/libs/core
sound.depends = src/libs/core

levels.path= /assets/levels
levels.files += $$files(levels/*.nel)
guitarLevels.path= /assets/levels/guitar
guitarLevels.files += $$files(levels/guitar/*.nel)
bassLevels.path= /assets/levels/bass-guitar
bassLevels.files += $$files(levels/bass-guitar/*.nel)
pianoLevels.path= /assets/levels/piano
pianoLevels.files += $$files(levels/piano/*.nel)

INSTALLS += levels guitarLevels bassLevels pianoLevels

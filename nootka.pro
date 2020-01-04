#=================================================================
# This file is part of Nootka (http://nootka.sf.net)
# Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)
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

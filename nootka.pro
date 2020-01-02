#=====================================================
# Main Nootka pro
#=====================================================

TARGET = nootka
TEMPLATE = subdirs

SUBDIRS = src/libs/core\
#           src/libs/mobile\
          src/libs/sound\
          src\


CONFIG += ordered

src.depends = src/libs/core
sound.depends = src/libs/core

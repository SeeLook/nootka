TEMPLATE = lib
TARGET = tartini
CONFIG -= qt
CONFIG += staticlib
VERSION = 0.1

HEADERS += \
  tartini_core.h \
  c_vector.h

SOURCES += \
  tartini_core.c \
  c_vector.c

LIBS += -lfftw3f -lpthread


QT += core gui widgets


TARGET = NootkaMobile
TEMPLATE = lib

SOURCES +=  ttouchmessage.cpp \
            tfiledialog.cpp \
            ttouchmenu.cpp \
            tmenuwidget.cpp \
            tmaterialmenu.cpp \
            tfingerpointer.cpp \

HEADERS  += ttouchmessage.h \
            tfiledialog.h \
            ttouchmenu.h \
            tmenuwidget.h \
            tmaterialmenu.h \
            tfingerpointer.h \

CONFIG += mobility warn_off
MOBILITY = 

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
}

LIBS += -L../core/ -lNootkaCore

INCLUDEPATH = ../core





QT       += widgets network

TARGET = NootkaUpdaterPlugin
TEMPLATE = lib


SOURCES +=  tupdatechecker.cpp\
            updatefunctions.cpp\
            tupdateruleswdg.cpp\
            tupdatesummary.cpp\

HEADERS +=  tupdatechecker.h\
            updatefunctions.h\
            tupdateruleswdg.h\
            tupdatesummary.h\

CONFIG += mobility warn_off
MOBILITY =

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
  ANDROID_EXTRA_LIBS += libNootkaUpdaterPlugin.so
}

INCLUDEPATH = ../../libs/core\

LIBS += -L../../libs/core/ -lNootkaCore\









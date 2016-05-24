
QT       += widgets

TARGET = NootkaAboutPlugin
TEMPLATE = lib


SOURCES +=  taboutplugin.cpp \
            taboutnootka.cpp \
            tsupportnootka.cpp \
            tdonorswidget.cpp \

HEADERS +=  taboutplugin.h \
            taboutnootka.h \
            tsupportnootka.h \
            tdonorswidget.h \

CONFIG += mobility warn_off
MOBILITY =

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
  ANDROID_EXTRA_LIBS += libNootkaAboutPlugin.so
}

INCLUDEPATH = ../../libs/core\
              ../../libs/misc\
              ../../libs/score\
              ../../libs/widgets\

LIBS += -L../../libs/core/ -lNootkaCore\
        -L../../libs/score/ -lNootkaScore\
        -L../../libs/widgets/ -lNootkaWidgets\
        -L../../libs/misc/ -lNootkaMisc\


RESOURCES += about.qrc







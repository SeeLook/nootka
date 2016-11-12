
QT       += widgets

TARGET = NootkaLevelPlugin
TEMPLATE = lib


SOURCES +=  tlevelplugin.cpp\
            tlevelcreatordlg.cpp\
            tlevelheaderwdg.cpp\
            tabstractlevelpage.cpp\
            accidsettings.cpp\
            tkeysigncombobox.cpp\
            levelsettings.cpp\
            questionssettings.cpp\
            tmelodysettings.cpp\
            rangesettings.cpp\

HEADERS +=  tlevelplugin.h\
            tlevelcreatordlg.h\
            tlevelheaderwdg.h\
            tabstractlevelpage.h\
            accidsettings.h\
            tkeysigncombobox.h\
            levelsettings.h\
            questionssettings.h\
            tmelodysettings.h\
            rangesettings.h\

CONFIG += mobility warn_off
MOBILITY = 

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
  ANDROID_EXTRA_LIBS += libNootkaLevelPlugin.so
}

INCLUDEPATH += ../../libs/core\
               ../../libs/mobile\
               ../../libs/score\
               ../../libs/widgets\
               ../../libs/sound\

LIBS += -L../../libs/core/ -lNootkaCore\
        -L../../libs/mobile/ -lNootkaMobile\
        -L../../libs/score/ -lNootkaScore\
        -L../../libs/widgets/ -lNootkaWidgets\
        -L../../libs/sound/ -lNootkaSound\

RESOURCES += level.qrc









QT       += widgets

TARGET = NootkaSettingsPlugin
TEMPLATE = lib


SOURCES += tsettingsplugin.cpp\
           tsettingsdialog.cpp\
           tglobalsettings.cpp\
           tscoresettings.cpp\
           tguitarsettings.cpp\
           audioinsettings.cpp\
           audiooutsettings.cpp\
           texamsettings.cpp\
           tlaysettings.cpp\
           tnotationradiogroup.cpp\
           tnotenamesettings.cpp\
           tcolorbutton.cpp\
           tlistmenu.cpp\
           tvolumeslider.cpp\
           tcolordialog.cpp\
           color_wheel.cpp\

HEADERS += tsettingsplugin.h\
           tsettingsdialog.h\
           tglobalsettings.h\
           tscoresettings.h\
           tguitarsettings.h\
           audioinsettings.h\
           audiooutsettings.h\
           texamsettings.h\
           tlaysettings.h\
           tnotationradiogroup.h\
           tnotenamesettings.h\
           tcolorbutton.h\
           tlistmenu.h\
           tvolumeslider.h\
           tcolordialog.h\
           color_wheel.hpp\
            
CONFIG += mobility warn_off
MOBILITY = 

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
  ANDROID_EXTRA_LIBS += libNootkaSettingsPlugin.so
}

INCLUDEPATH = ../../libs/core\
              ../../libs/mobile\
              ../../libs/score\
              ../../libs/widgets\
              ../../libs/sound\
              ../../libs/misc\

LIBS += -L../../libs/core/ -lNootkaCore\
        -L../../libs/mobile/ -lNootkaMobile\
        -L../../libs/score/ -lNootkaScore\
        -L../../libs/widgets/ -lNootkaWidgets\
        -L../../libs/sound/ -lNootkaSound\
        -L../../libs/misc/ -lNootkaMisc\

RESOURCES += settings.qrc








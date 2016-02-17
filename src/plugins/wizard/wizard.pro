
QT       += widgets

TARGET = NootkaWizardPlugin
TEMPLATE = lib


SOURCES +=  twizardplugin.cpp\
            tfirstrunwizard.cpp\

HEADERS +=  twizardplugin.h\
            tfirstrunwizard.h\

CONFIG += mobility warn_off
MOBILITY =

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
  ANDROID_EXTRA_LIBS += libNootkaWizardPlugin.so
}

INCLUDEPATH = ../../libs/core\
              ../../libs/mobile\
              ../../libs/score\
              ../../libs/widgets\
              ../../libs/misc\

LIBS += -L../../libs/core/ -lNootkaCore\
        -L../../libs/mobile/ -lNootkaMobile\
        -L../../libs/score/ -lNootkaScore\
        -L../../libs/widgets/ -lNootkaWidgets\
        -L../../libs/misc/ -lNootkaMisc\


# RESOURCES += wizard.qrc







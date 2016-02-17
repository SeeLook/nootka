
QT       += widgets printsupport

TARGET = NootkaExamPlugin
TEMPLATE = lib


SOURCES +=  texamplugin.cpp \
            tcanvas.cpp \
            tnootkacertificate.cpp \
            tquestiontip.cpp \
            tstartexamdlg.cpp \
            texamexecutor.cpp \
            texecutorsupply.cpp \
            texercises.cpp \
            tglobalexamstore.cpp \
            texamsummary.cpp \
            tequalrand.cpp \
            tpenalty.cpp \
            texammelody.cpp \

HEADERS += texamplugin.h \
            tcanvas.h \
            tnootkacertificate.h \
            tquestiontip.h \
            tstartexamdlg.h \
            texamexecutor.h \
            texecutorsupply.h \
            texercises.h \
            tglobalexamstore.h \
            texamsummary.h \
            tequalrand.h \
            tpenalty.h \
            texammelody.h \

CONFIG += mobility warn_off
MOBILITY = 

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
  ANDROID_EXTRA_LIBS += libNootkaExamPlugin.so
}

INCLUDEPATH = ../../libs/core\
              ../../libs/mobile\
              ../../libs/score\
              ../../libs/widgets\
              ../../libs/sound\
              ../../libs/main\
              ../../libs/misc\

LIBS += -L../../libs/core/ -lNootkaCore\
        -L../../libs/mobile/ -lNootkaMobile\
        -L../../libs/score/ -lNootkaScore\
        -L../../libs/widgets/ -lNootkaWidgets\
        -L../../libs/sound/ -lNootkaSound\
        -L../../libs/main/ -lNootkaMain\
        -L../../libs/misc/ -lNootkaMisc\









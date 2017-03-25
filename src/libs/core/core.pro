
QT += core gui androidextras quick quickcontrols2


TARGET = NootkaCore
TEMPLATE = lib

SOURCES +=  tcolor.cpp\
            tfingerpos.cpp\
            tglobals.cpp\
            tinitcorelib.cpp\
#             tnoofont.cpp\
            tpath.cpp\
#             tmtr.cpp\
            tnootkaqml.cpp\
            ttickcolors.cpp\
            taction.cpp\
          \
#             exam/tattempt.cpp\
#             exam/texam.cpp\
#             exam/tlevel.cpp\
#             exam/tqagroup.cpp\
#             exam/tqatype.cpp\
#             exam/tqaunit.cpp\
#             exam/tresulttext.cpp\
          \
            music/tchunk.cpp\
            music/tclef.cpp\
            music/tinstrument.cpp\
            music/tkeysignature.cpp\
            music/tmeasure.cpp\
            music/tmelody.cpp\
            music/tnamestylefilter.cpp\
            music/tnote.cpp\
            music/ttune.cpp\
            music/trhythm.cpp\
            music/tmeter.cpp\
          \
            score/tscoreobject.cpp\
            score/tstaffobject.cpp\
            score/tmeasureobject.cpp\
            score/tnoteobject.cpp\
            score/tnotepair.cpp\
            score/tbeamobject.cpp\
          \
            instruments/tguitarbg.cpp\
          \
            Android/tandroid.cpp\

HEADERS  += nootkaconfig.h\
            texamparams.h\
            taudioparams.h\
            tcolor.h\
            texamparams.h\
            tfingerpos.h\
            tglobals.h\
            tinitcorelib.h\
            tnoofont.h\
            tpath.h\
            tscoreparams.h\
#             tmtr.h\
            tnootkaqml.h\
            ttickcolors.h\
            taction.h\
          \
#             exam/tattempt.h\
#             exam/texam.h\
#             exam/textrans.h\
#             exam/tlevel.h\
#             exam/tqagroup.h\
#             exam/tqatype.h\
#             exam/tqaunit.h\
#             exam/tresulttext.h\
          \
            music/tchunk.h\
            music/tclef.h\
            music/tinstrument.h\
            music/tkeysignature.h\
            music/tmeasure.h\
            music/tmelody.h\
            music/tmeter.h\
            music/tnamestylefilter.h\
            music/tnote.h\
            music/tnotestruct.h\
            music/trhythm.h\
            music/ttune.h\
          \
            score/tscoreobject.h\
            score/tstaffobject.h\
            score/tmeasureobject.h\
            score/tnoteobject.h\
            score/tnotepair.h\
            score/tbeamobject.h\
          \
            instruments/tguitarbg.h\
            Android/tandroid.h\

CONFIG += mobility warn_off
MOBILITY = 

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
}

# INCLUDEPATH = core


RESOURCES += core.qrc


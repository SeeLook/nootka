
QT += core widgets androidextras quick quickcontrols2


TARGET = NootkaCore
TEMPLATE = lib

SOURCES +=  tcolor.cpp\
            tfingerpos.cpp\
            tglobals.cpp\
            tinitcorelib.cpp\
#             tnoofont.cpp\
            tpath.cpp\
            tmtr.cpp\
            tnootkaqml.cpp\
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
            music/ttuneobject.cpp\
            music/trhythm.cpp\
            music/tmeter.cpp\
          \
            score/tscoreobject.cpp\
            score/tstaffobject.cpp\
            score/tmeasureobject.cpp\
            score/tnoteobject.cpp\
            score/tnotepair.cpp\
            score/tbeamobject.cpp\
            score/tstafflines.cpp\
            score/taddobject.cpp\
          \
            instruments/tcommoninstrument.cpp\
            instruments/tguitarbg.cpp\
            instruments/tpianobg.cpp\
            instruments/tbandoneonbg.cpp\
            instruments/tsaxbg.cpp\
          \
            Android/tandroid.cpp\
            Android/tfiledialog.cpp\
            Android/tmobilemenu.cpp\

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
            tmtr.h\
            tnootkaqml.h\
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
            music/ttuneobject.h\
          \
            score/tscoreobject.h\
            score/tstaffobject.h\
            score/tmeasureobject.h\
            score/tnoteobject.h\
            score/tnotepair.h\
            score/tbeamobject.h\
            score/tstafflines.h\
            score/taddobject.h\
          \
            instruments/tcommoninstrument.h\
            instruments/tguitarbg.h\
            instruments/tpianobg.h\
            instruments/tbandoneonbg.h\
            instruments/tsaxbg.h\
          \
            Android/tandroid.h\
            Android/tfiledialog.h\
            Android/tmobilemenu.h\

CONFIG += mobility warn_off
MOBILITY = 

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
}

# INCLUDEPATH = core


RESOURCES += core.qrc


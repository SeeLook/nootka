#=================================================================
# This file is part of Nootka (http://nootka.sf.net)
# Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)
# on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)
#
# core library on Nootka
#=================================================================


QT += core widgets androidextras quick quickcontrols2


TARGET = NootkaCore
TEMPLATE = lib

SOURCES +=  tinitcorelib.cpp \
            tcolor.cpp \
            tfingerpos.cpp \
            tglobals.cpp \
            tpath.cpp \
            tmtr.cpp \
            tnootkaqml.cpp \
            taction.cpp \
            tnoofont.cpp \
          \
            music/tnote.cpp \
            music/trhythm.cpp \
            music/tnamestylefilter.cpp \
            music/ttune.cpp \
            music/ttuneobject.cpp \
            music/tclef.cpp \
            music/tinstrument.cpp \
            music/tkeysignature.cpp \
            music/tchunk.cpp \
            music/tmeasure.cpp \
            music/tmelody.cpp \
            music/tmeter.cpp \
            music/tnotestruct.cpp \
            music/ttechnical.cpp \
            music/trtmgroup.cpp \
          \
            score/tscoreobject.cpp \
            score/tstaffitem.cpp \
            score/tmeasureobject.cpp \
            score/tnoteitem.cpp \
            score/tnotepair.cpp \
            score/tbeamobject.cpp \
            score/tstafflines.cpp \
            score/taddnoteitem.cpp \
          \
            instruments/tcommoninstrument.cpp \
            instruments/tguitarbg.cpp \
            instruments/tpianobg.cpp \
            instruments/tbandoneonbg.cpp \
            instruments/tsaxbg.cpp \
          \
            exam/tqatype.cpp \
            exam/tqaunit.cpp \
            exam/tqagroup.cpp \
            exam/tattempt.cpp \
            exam/tlevel.cpp \
            exam/texam.cpp \
            exam/tresulttext.cpp \
          \
            Android/tandroid.cpp \
            Android/tfiledialog.cpp \

HEADERS  += nootkaconfig.h \
            texamparams.h \
            taudioparams.h \
            tscoreparams.h \
            tinitcorelib.h \
            tcolor.h \
            tfingerpos.h \
            tglobals.h \
            tpath.h \
            tmtr.h \
            tnootkaqml.h \
            taction.h \
            tnoofont.h \
          \
            music/tnote.h \
            music/trhythm.h \
            music/tnamestylefilter.h \
            music/ttune.h \
            music/ttuneobject.h \
            music/tclef.h \
            music/tinstrument.h \
            music/tkeysignature.h \
            music/tchunk.h \
            music/tmeasure.h \
            music/tmelody.h \
            music/tmeter.h \
            music/tnotestruct.h \
            music/ttechnical.h \
            music/trtmgroup.h \
          \
            score/tscoreobject.h \
            score/tstaffitem.h \
            score/tmeasureobject.h \
            score/tnoteitem.h \
            score/tnotepair.h \
            score/tbeamobject.h \
            score/tstafflines.h \
            score/taddnoteitem.h \
          \
            instruments/tcommoninstrument.h \
            instruments/tguitarbg.h \
            instruments/tpianobg.h \
            instruments/tbandoneonbg.h \
            instruments/tsaxbg.h \
          \
            exam/tqatype.h \
            exam/tqaunit.h \
            exam/tqagroup.h \
            exam/tattempt.h \
            exam/tlevel.h \
            exam/texam.h \
            exam/textrans.h \
            exam/tresulttext.h \
          \
            Android/tandroid.h \
            Android/tfiledialog.h \


CONFIG += mobility warn_off
MOBILITY = 

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
}

# INCLUDEPATH = core


RESOURCES += core.qrc


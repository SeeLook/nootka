
QT += core gui widgets androidextras


TARGET = NootkaCore
TEMPLATE = lib

SOURCES +=  tcolor.cpp\
            tfingerpos.cpp\
            tglobals.cpp\
            tinitcorelib.cpp\
            tnoofont.cpp\
            tpath.cpp\
            tmtr.cpp\
          \
            animations/tabstractanim.cpp\
            animations/tblinkingitem.cpp\
            animations/tcoloredanim.cpp\
            animations/tcombinedanim.cpp\
            animations/tcrossfadetextanim.cpp\
            animations/tfadeanim.cpp\
            animations/tmorphedanim.cpp\
            animations/tmovedanim.cpp\
            animations/tscaledanim.cpp\
          \
            exam/tattempt.cpp\
            exam/texam.cpp\
            exam/tlevel.cpp\
            exam/tqagroup.cpp\
            exam/tqatype.cpp\
            exam/tqaunit.cpp\
            exam/tresulttext.cpp\
          \
            graphics/tdropshadoweffect.cpp\
            graphics/tgraphicstexttip.cpp\
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
          \
            widgets/tpushbutton.cpp\
          \
            plugins/tpluginsloader.cpp\
          \
            touch/ttouchproxy.cpp\
#             touch/ttouchmenu.cpp\ 
            touch/ttoucharea.cpp\
#             touch/tmenuwidget.cpp\
            touch/ttouchparams.cpp\
#             touch/ttouchmessage.cpp\
          \
            Android/tandroid.cpp\

HEADERS  += nootkaconfig.h\
            tprecisetimer.h\
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
          \
            animations/tabstractanim.h\
            animations/tblinkingitem.h\
            animations/tcoloredanim.h\
            animations/tcombinedanim.h\
            animations/tcrossfadetextanim.h\
            animations/tfadeanim.h\
            animations/tmorphedanim.h\
            animations/tmovedanim.h\
            animations/tscaledanim.h\
          \
            exam/tattempt.h\
            exam/texam.h\
            exam/textrans.h\
            exam/tlevel.h\
            exam/tqagroup.h\
            exam/tqatype.h\
            exam/tqaunit.h\
            exam/tresulttext.h\
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
            graphics/tdropshadoweffect.h\
            graphics/tgraphicstexttip.h\
          \
            widgets/tpushbutton.h\
          \
            plugins/tplugininterface.h\
            plugins/tpluginsloader.h\
          \
            touch/ttouchproxy.h\
#             touch/ttouchmenu.h\
            touch/ttoucharea.h\
#             touch/tmenuwidget.h\
            touch/ttouchparams.h\
#             touch/ttouchmessage.h\
            Android/tandroid.h\
            
CONFIG += mobility warn_off
MOBILITY = 

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
#  ANDROID_EXTRA_LIBS += libNootkaCore.so
}

# INCLUDEPATH = core


RESOURCES += core.qrc


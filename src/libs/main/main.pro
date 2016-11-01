
QT += core gui widgets


TARGET = NootkaMain
TEMPLATE = lib

SOURCES +=  animations/tstrikedoutitem.cpp \
          \
            gui/tmainview.cpp \
            gui/tmelman.cpp \
            gui/ttoolbar.cpp \
            gui/tmenu.cpp \
            gui/tstatuslabel.cpp \
            gui/tbgpixmap.cpp \
          \
            guitar/tfingerboard.cpp \
            guitar/tguitarview.cpp \
          \
            notename/tnotename.cpp \
            notename/tnotenamelabel.cpp \
            notename/tnametip.cpp \
          \
            score/tmainscore.cpp \
            score/tscoreactions.cpp \
          \
            exam/tprogresswidget.cpp \
            exam/texamview.cpp \
            exam/trandmelody.cpp \

HEADERS  += animations/tstrikedoutitem.h \
          \
            gui/tmainview.h \
            gui/tmelman.h \
            gui/ttoolbar.h \
            gui/tmenu.h \
            gui/tstatuslabel.h \
            gui/tbgpixmap.h \
          \
            guitar/tfingerboard.h \
            guitar/tguitarview.h \
          \
            notename/tnotename.h \
            notename/tnotenamelabel.h \
            notename/tnametip.h \
          \
            score/tmainscore.h \
            score/tscoreactions.h \
          \
            exam/tprogresswidget.h \
            exam/texamview.h \
            exam/trandmelody.h \

CONFIG += mobility warn_off
MOBILITY = 

QMAKE_CXXFLAGS_RELEASE += -fsigned-char
QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1

LIBS += -L../core/ -lNootkaCore \
        -L../mobile/ -lNootkaMobile \
        -L../score/ -lNootkaScore \
        -L../widgets -lNootkaWidgets \
        -L../sound -lNootkaSound

INCLUDEPATH += ../core ../mobile ../score ../widgets ../sound




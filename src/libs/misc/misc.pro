
QT += core gui widgets


TARGET = NootkaMisc
TEMPLATE = lib

SOURCES +=  tscalepreviewlabel.cpp\
            select7note.cpp\
            tnootkalabel.cpp\
            tabout.cpp\
            \
            help/texamhelp.cpp\
            help/texpertanswerhelp.cpp\
            help/tmainhelp.cpp\

HEADERS  += nootkamiscglobal.h\
            tmisctrans.h\
            tscalepreviewlabel.h\
            select7note.h\
            tnootkalabel.h\
            tabout.h\
          \
            help/texamhelp.h\
            help/texpertanswerhelp.h\
            help/tmainhelp.h\

CONFIG += mobility warn_off
MOBILITY = 

QMAKE_CXXFLAGS_RELEASE += -fsigned-char
QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1

LIBS += -L../core/ -lNootkaCore -L../mobile/ -lNootkaMobile -L../score/ -lNootkaScore -L../widgets -lNootkaWidgets

INCLUDEPATH += ../core ../mobile ../score ../widgets

# append Qt base translations from corrent Qt installation
TR_DIR = "$$system(dirname $$QMAKESPEC)/../translations"

lang.path = /assets/lang
lang.files += lang/nootka_pl.qm
lang.files += lang/nootka_cs.qm
lang.files += lang/nootka_de.qm
lang.files += lang/nootka_fr.qm
lang.files += lang/nootka_ru.qm
lang.files += lang/nootka_es.qm
lang.files += $$system(ls $$TR_DIR/qtbase_cs.qm)
lang.files += $$system(ls $$TR_DIR/qtbase_de.qm)
lang.files += $$system(ls $$TR_DIR/qtbase_fr.qm)
lang.files += $$system(ls $$TR_DIR/qtbase_pl.qm)
lang.files += $$system(ls $$TR_DIR/qtbase_ru.qm)
lang.files += lang/qtbase_es.qm

lang.depends += FORCE

INSTALLS += lang

#RESOURCES += misc.qrc # unused so far



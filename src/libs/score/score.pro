
QT += core gui widgets


TARGET = NootkaScore
TEMPLATE = lib

SOURCES +=  graphics/tnotepixmap.cpp \
          \
            widgets/tselectclef.cpp \
          \
            tscoreitem.cpp \
            tscoreclef.cpp \
            tscorekeysignature.cpp \
            tscorenote.cpp \
            tscorescene.cpp \
            tscorescordature.cpp \
            tscorestaff.cpp \
            tscore5lines.cpp \
            tnotecontrol.cpp \
            tscorelines.cpp \
            tsimplescore.cpp \
            tmultiscore.cpp \

HEADERS  += graphics/tnotepixmap.h \
          \
            widgets/tselectclef.h \
          \
            tscoreitem.h \
            tscoreclef.h \
            tscorekeysignature.h \
            tscorenote.h \
            tscorescene.h \
            tscorescordature.h \
            tscorestaff.h \
            tscore5lines.h \
            tnotecontrol.h \
            tscorelines.h \
            tsimplescore.h \
            tmultiscore.h \

CONFIG += mobility warn_off
MOBILITY = 

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
}

LIBS += -L../core/ -lNootkaCore -L../mobile/ -lNootkaMobile

INCLUDEPATH = ../core ../mobile




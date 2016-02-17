
QT += core gui widgets


TARGET = NootkaWidgets
TEMPLATE = lib

SOURCES +=  level/tlevelsdefs.cpp \
            level/tlevelpreview.cpp \
            level/tlevelselector.cpp \
            level/tfixleveldialog.cpp \
          \
            widgets/tquestionaswdg.cpp \
            widgets/tsettingsdialogbase.cpp \
            widgets/troundedlabel.cpp \
            widgets/tselectinstrument.cpp \

HEADERS  += level/tlevelsdefs.h \
            level/tlevelpreview.h \
            level/tlevelselector.h \
            level/tfixleveldialog.h \
          \
            widgets/tquestionaswdg.h \
            widgets/tsettingsdialogbase.h \
            widgets/troundedlabel.h \
            widgets/tselectinstrument.h \

CONFIG += mobility warn_off
MOBILITY = 

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
}

LIBS += -L../core/ -lNootkaCore -L../mobile/ -lNootkaMobile -L../score/ -lNootkaScore

INCLUDEPATH = ../core ../mobile ../score




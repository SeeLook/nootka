#=================================================================
# This file is part of Nootka (http://nootka.sf.net)
# Copyright (C) 2017-2021 by Tomasz Bojczuk (seelook@gmail.com)
# on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)
#
# Main Nootka executable
#=================================================================


QT += multimedia androidextras core widgets quick quickcontrols2 printsupport


TARGET = Nootka
TEMPLATE = app

CONFIG += lrelease mobility warn_off

SOURCES +=  main.cpp \
            mobile/tmobilemenu.cpp \
          \
            main/tnameitem.cpp \
            main/texamexecutor.cpp \
            main/texecutorsupply.cpp \
            main/tglobalexamstore.cpp \
            main/texercises.cpp \
            main/tequalrand.cpp \
            main/texammelody.cpp \
            main/trandmelody.cpp \
            main/texamview.cpp \
            main/tpenalty.cpp \
            main/ttiphandler.cpp \
            main/tmainscoreobject.cpp \
            main/texamsummary.cpp \
            main/tstartexamitem.cpp \
            main/tnootkacertificate.cpp \
            main/tpiechartitem.cpp \
            main/tnotetoplay.cpp \
            main/tgotit.cpp \
          \
            dialogs/tdialogloaderobject.cpp \
            dialogs/tlevelcreatoritem.cpp \
            dialogs/tlevelpreviewitem.cpp \
            dialogs/tlevelsdefs.cpp \
            dialogs/tlevelselector.cpp \
            dialogs/tmelodylistview.cpp \
            dialogs/tmelodywrapper.cpp \
            dialogs/ttunerdialogitem.cpp \
            dialogs/trtmselectoritem.cpp \
            dialogs/tmelgenitem.cpp \
            dialogs/tmelodyimportitem.cpp \
          \
            help/texamhelp.cpp \
            help/tmainhelp.cpp \
          \
            updater/tupdatechecker.cpp \
            updater/updatefunctions.cpp \
            updater/tupdateitem.cpp \
          \

HEADERS  += mobile/tmobilemenu.h \
          \
            main/tnameitem.h \
            main/texamexecutor.h \
            main/texecutorsupply.h \
            main/tglobalexamstore.h \
            main/texercises.h \
            main/tequalrand.h \
            main/texammelody.h \
            main/trandmelody.h \
            main/texamview.h \
            main/tpenalty.h \
            main/ttiphandler.h \
            main/tmainscoreobject.h \
            main/texamsummary.h \
            main/tstartexamitem.h \
            main/tnootkacertificate.h \
            main/tpiechartitem.h \
            main/tnotetoplay.h \
            main/tgotit.h \
          \
            dialogs/tdialogloaderobject.h \
            dialogs/tlevelcreatoritem.h \
            dialogs/tlevelpreviewitem.h \
            dialogs/tlevelsdefs.h \
            dialogs/tlevelselector.h \
            dialogs/tmelodylistview.h \
            dialogs/tmelodywrapper.h \
            dialogs/ttunerdialogitem.h \
            dialogs/trtmselectoritem.h \
            dialogs/tmelgenitem.h \
            dialogs/tmelodyimportitem.h \
          \
            help/texamhelp.h \
            help/tmainhelp.h \
          \
            updater/tupdatechecker.h \
            updater/updatefunctions.h \
            updater/tupdateitem.h \
          \

MOBILITY = 

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
}

INCLUDEPATH += libs/core libs/mobile libs/sound libs/main

RESOURCES += nootka-android.qrc

versionAtLeast(QT_VERSION, 5.15.0) {
  DEFINES += QT_NO_DEPRECATED_WARNINGS

  LIBS += -Llibs/core/ -lNootkaCore_$${QT_ARCH} \
          -Llibs/sound -lNootkaSound_$${QT_ARCH} \

  DISTFILES += \
    android21/AndroidManifest.xml \
    android21/res/values/libs.xml \
    android21/build.gradle \
    android21/gradle/wrapper/gradle-wrapper.jar \
    android21/gradlew \
    android21/gradle/wrapper/gradle-wrapper.properties \
    android21/gradlew.bat \
    android21/net/sf/nootka/TshareExam.java \
    android21/net/sf/nootka/ToutVolume.java \

  ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android21

} else {

  LIBS += -Llibs/core/ -lNootkaCore \
          -Llibs/sound -lNootkaSound \

  DISTFILES += \
    android/AndroidManifest.xml \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/net/sf/nootka/TshareExam.java \
    android/net/sf/nootka/ToutVolume.java \

  ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
}


# ======== Translation files ==============
# EXTRA_TRANSLATIONS is qmake variable that makes all *.ts files converted into *.qm
# inside LRELEASE_DIR of build path (OUT_PWD) during build.
LRELEASE_DIR = lang
EXTRA_TRANSLATIONS += $$files(lang/nootka_*.ts)
EXTRA_TRANSLATIONS += lang/qtbase_sl.ts

lang.path = /assets/lang
# HACK: create list of translated file names from the list of existing *.ts files.
# and append to lang file list - they don't exist at first launch of qmake.
# They are created only during first compilation.
# But at the end, when install target works, it can place them properly in Android assets dir.
QM_STAFF = $$replace(EXTRA_TRANSLATIONS, ts, qm)
versionAtLeast(QT_VERSION, 5.15.0) {
  lang.files += $$replace(QM_STAFF, lang, "$$OUT_PWD/lang/$${QT_ARCH}")
} else {
  lang.files += $$replace(QM_STAFF, lang, "$$OUT_PWD/lang")
}

# Then append Qt base translations from current Qt installation
TR_DIR = "$$system(dirname $$QMAKESPEC)/../translations"
lang.files += $$system(ls $$TR_DIR/qtbase_cs.qm)
lang.files += $$system(ls $$TR_DIR/qtbase_de.qm)
lang.files += $$system(ls $$TR_DIR/qtbase_es.qm)
lang.files += $$system(ls $$TR_DIR/qtbase_fr.qm)
lang.files += $$system(ls $$TR_DIR/qtbase_hu.qm)
lang.files += $$system(ls $$TR_DIR/qtbase_it.qm)
lang.files += $$system(ls $$TR_DIR/qtbase_pl.qm)
lang.files += $$system(ls $$TR_DIR/qtbase_ru.qm)
lang.files += $$system(ls $$TR_DIR/qtbase_uk.qm)
lang.CONFIG += no_check_exist

lang.depends += FORCE

INSTALLS += lang

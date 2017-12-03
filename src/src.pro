
QT += multimedia androidextras core widgets quick quickcontrols2 printsupport


TARGET = Nootka2
TEMPLATE = app

SOURCES += main.cpp \
           main/tnameitem.cpp\
          \
           dialogs/tdialogloaderobject.cpp\
           dialogs/tlevelcreatoritem.cpp\
           dialogs/tlevelpreviewitem.cpp\
           dialogs/tlevelsdefs.cpp\
           dialogs/tlevelselector.cpp\
           dialogs/tlevelheaderwdg.cpp\

HEADERS  += main/tnameitem.h\
          \
           dialogs/tdialogloaderobject.h\
           dialogs/tlevelcreatoritem.h\
           dialogs/tlevelpreviewitem.h\
           dialogs/tlevelsdefs.h\
           dialogs/tlevelselector.h\
           dialogs/tlevelheaderwdg.h\



CONFIG += mobility warn_off
MOBILITY = 

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
  ANDROID_EXTRA_LIBS += $$OUT_PWD/libs/core/libNootkaCore.so\
#                         $$OUT_PWD/libs/mobile/libNootkaMobile.so\
                        $$OUT_PWD/libs/sound/libNootkaSound.so\

}

INCLUDEPATH += libs/core libs/mobile libs/sound libs/main

LIBS += -Llibs/core/ -lNootkaCore \
#         -Llibs/mobile/ -lNootkaMobile \
        -Llibs/sound -lNootkaSound \

RESOURCES += nootka.qrc

DISTFILES += \
    android/AndroidManifest.xml \
    android/res/values/libs.xml \
    android/build.gradle \
    android/gradle/wrapper/gradle-wrapper.jar \
    android/gradlew \
    android/gradle/wrapper/gradle-wrapper.properties \
    android/gradlew.bat \
    android/net/sf/nootka/TshareExam.java\
    android/net/sf/nootka/ToutVolume.java\


ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

# append Qt base translations from current Qt installation
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
lang.files += $$system(ls $$TR_DIR/qtbase_es.qm)
lang.files += $$system(ls $$TR_DIR/qtbase_fr.qm)
lang.files += $$system(ls $$TR_DIR/qtbase_pl.qm)
lang.files += $$system(ls $$TR_DIR/qtbase_ru.qm)

lang.depends += FORCE

INSTALLS += lang



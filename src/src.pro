
QT += multimedia androidextras core gui widgets printsupport


TARGET = Nootka
TEMPLATE = app

SOURCES += main.cpp \
           mainwindow.cpp \
           ttouchstyle.cpp \

HEADERS  += mainwindow.h \
           ttouchstyle.h \
          \


CONFIG += mobility warn_off
MOBILITY = 

android {
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char
  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1
  ANDROID_EXTRA_LIBS += $$OUT_PWD/libs/core/libNootkaCore.so\
                        $$OUT_PWD/libs/mobile/libNootkaMobile.so\
                        $$OUT_PWD/libs/score/libNootkaScore.so\
                        $$OUT_PWD/libs/widgets/libNootkaWidgets.so\
                        $$OUT_PWD/libs/sound/libNootkaSound.so\
                        $$OUT_PWD/libs/misc/libNootkaMisc.so\
                        $$OUT_PWD/libs/main/libNootkaMain.so\
}

INCLUDEPATH = INCLUDEPATH += libs/core libs/mobile libs/score libs/widgets libs/sound libs/main

LIBS += -Llibs/core/ -lNootkaCore \
        -Llibs/mobile/ -lNootkaMobile \
        -Llibs/score/ -lNootkaScore \
        -Llibs/widgets -lNootkaWidgets \
        -Llibs/sound -lNootkaSound \
        -Llibs/main -lNootkaMain

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


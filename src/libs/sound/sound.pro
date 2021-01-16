#=================================================================
# This file is part of Nootka (http://nootka.sf.net)
# Copyright (C) 2017-2020 by Tomasz Bojczuk (seelook@gmail.com)
# on the terms of GNU GPLv3 license (http://www.gnu.org/licenses)
#
# sound Nootka library
#=================================================================


QT += core multimedia gui widgets androidextras quick quickcontrols2


TARGET = NootkaSound
TEMPLATE = lib

SOURCES +=  tabstractplayer.cpp\
            tpitchfinder.cpp\
            tonsetlogic.cpp\
            tcommonlistener.cpp\
            tqtaudioin.cpp\
            tqtaudioout.cpp\
            toggscale.cpp\
            tsound.cpp\
            ttickcolors.cpp\
            \
            tartini/analysisdata.cpp\
            tartini/bspline.cpp\
            tartini/channel.cpp\
            tartini/conversions.cpp\
            tartini/fast_smooth.cpp\
            tartini/mytransforms.cpp\
            tartini/notedata.cpp\
            tartini/useful.cpp\
            tartini/filters/FastSmoothedAveragingFilter.cpp\
            tartini/filters/FixedAveragingFilter.cpp\
            tartini/filters/GrowingAveragingFilter.cpp\
            tartini/filters/IIR_Filter.cpp\
            \
            stouch/SoundTouch.cpp\
            stouch/TDStretch.cpp\
            stouch/RateTransposer.cpp\
            stouch/AAFilter.cpp\
            stouch/FIRFilter.cpp\ 
            stouch/FIFOSampleBuffer.cpp\
            stouch/cpu_detect_x86.cpp\
            stouch/sse_optimized.cpp\
            stouch/InterpolateCubic.cpp\
            stouch/InterpolateLinear.cpp\
            stouch/InterpolateShannon.cpp\

HEADERS  += tartiniparams.h\
            taudioobject.h\
            tabstractplayer.h\
            tpitchfinder.h\
            tonsetlogic.h\
            tcommonlistener.h\
            tqtaudioin.h\
            tqtaudioout.h\
            toggscale.h\
            tsound.h\
            taudiobuffer.h\
            ttickcolors.h\
            \
            tartini/array1d.h\
            tartini/large_vector.h\
            tartini/SmartPtr.h\
            tartini/filters/Filter.h\
            tartini/analysisdata.h\
            tartini/bspline.h\
            tartini/channel.h\
            tartini/conversions.h\
            tartini/fast_smooth.h\
            tartini/mytransforms.h\
            tartini/notedata.h\
            tartini/useful.h\
            tartini/filters/FastSmoothedAveragingFilter.h\
            tartini/filters/FixedAveragingFilter.h\
            tartini/filters/GrowingAveragingFilter.h\
            tartini/filters/IIR_Filter.h\
            \
            stouch\STTypes.h\
            stouch/SoundTouch.h\
            stouch/TDStretch.h\
            stouch/RateTransposer.h\
            stouch/AAFilter.h\
            stouch/FIRFilter.h\
            stouch/FIFOSampleBuffer.h\
            stouch/cpu_detect.h\
            stouch/InterpolateCubic.h\
            stouch/InterpolateLinear.h\
            stouch/InterpolateShannon.h\

CONFIG += mobility warn_off
MOBILITY = 


android {
# fftw and ogg are symbolic links point to those libraries built statically
# - for supported mobile architectures.
# They contain appropriate folders: arm7, x86, arm64

contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
    LIBS += -Bstatic -L$$PWD/fftw/armv7/ -lfftw3f -L$$PWD/ogg/armv7/ -loggstatic -Bdynamic
  }

  contains(ANDROID_TARGET_ARCH,arm64-v8a) {
    LIBS += -Bstatic -L$$PWD/fftw/arm64/ -lfftw3f -L$$PWD/ogg/arm64/ -loggstatic -Bdynamic
  }

  contains(ANDROID_TARGET_ARCH,x86) {
    LIBS += -Bstatic -L$$PWD/fftw/x86/ -lfftw3f -L$$PWD/ogg/x86/ -loggstatic -Bdynamic
  }

  contains(ANDROID_TARGET_ARCH,x86_64) {
    LIBS += -Bstatic -L$$PWD/fftw/x86_64/ -lfftw3f -L$$PWD/ogg/x86_64/ -loggstatic -Bdynamic
  }
  
  QMAKE_CXXFLAGS_RELEASE -= -O0
  QMAKE_CXXFLAGS_RELEASE -= -O1
  QMAKE_CXXFLAGS_RELEASE -= -O2
  QMAKE_CXXFLAGS_RELEASE += -fsigned-char -O3

  QMAKE_CXXFLAGS_DEBUG += -fsigned-char -O1

# There are links to folders with apparent headers due to Android environment does not includes standard /usr/include 
  INCLUDEPATH += fftw ogg vorbis
}

LIBS += -L../core/ -lNootkaCore  #-L../mobile/ -lNootkaMobile

INCLUDEPATH += ../core #../mobile

sounds.path = /assets/sounds
#sounds.files += sounds/alto-sax.ogg
#sounds.files += sounds/bandoneon.ogg
sounds.files += sounds/bass-guitar.ogg
sounds.files += sounds/classical-guitar.ogg
sounds.files += sounds/electric-guitar.ogg
sounds.files += sounds/piano.ogg
#sounds.files += sounds/tenor-sax.ogg
sounds.files += sounds/beat.raw48-16
sounds.depends += FORCE

INSTALLS += sounds


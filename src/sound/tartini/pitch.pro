#Tartini 1.1 QMakefile
#Usage:
#[]$ qmake pitch.pro
#[]$ make

#Set the paths to Qt4, fftw and qwt libraries and includes here.
#for you platform. Note multiple paths can be listed.
#Note the -L before the each library paths, but not include paths.

unix{
  macx{ #MacOSX
    MY_LIB_PATH += -L/Users/student/usr/local/lib
    MY_INCLUDE_PATH += /Users/student/usr/local/include
  }else{ #Linux
    MY_LIB_PATH += -L/usr/lib
    MY_INCLUDE_PATH += /usr/include/qwt-qt4
  }
}
win32{ #Windows
  MY_LIB_PATH += -L"C:/home/Qwt/lib" -L"C:/home/fftw"
  MY_INCLUDE_PATH += "./" "C:/home/Qwt/include" "C:/home/fftw"
  #Note just stick the *.dll files in the executables directory to run
}


TEMPLATE = app
CONFIG += qt thread opengl
CONFIG -= debug
CONFIG += release
CONFIG += warn_on
CONFIG += precompile_header
#CONFIG += profile
#DEFINES += SHARED_DRAWING_BUFFER
DEFINES += SINGLE_DRAWING_BUFFER
LANGUAGE = C++
TARGET = tartini
VERSION = 1.2.0

FORMS += dialogs/settingsdialog.ui

DISTFILES += INSTALL.txt \
  LICENSE.txt \
  README.txt \
  pics/open.xpm \
  pics/close32x32.xpm \
  pics/beginning32x32.xpm \
  pics/rewind32x32.xpm \
  pics/stop32x32.xpm \
  pics/play32x32.xpm \
  pics/fastforward32x32.xpm \
  pics/end32x32.xpm \
  pics/autofollow32x32.xpm \
  pics/shadingon32x32.xpm \
  pics/shadingoff32x32.xpm \
  pics/chain_open.xpm \
  pics/chain_closed.xpm \
  pics/record32x32.xpm \
  pics/playrecord32x32.xpm \
  pics/save32x32.xpm \
  pics/background.xpm \
  pics/zoomx.xpm \
  pics/zoomy.xpm \
  pics/zoomxout.xpm \
  pics/zoomyout.xpm \
  pics/tartinilogo.jpg \
  gomacx \
  gowindows.bat \
  widgets/base/baseview.h \
  widgets/base/baseview.cpp \
  widgets/base/basewidget.cpp \
  widgets/base/basewidget.h \
  include/static.h

HEADERS += main.h \
  sound/sound_stream.h \
  sound/wave_stream.h \
  sound/sound_file_stream.h \
  sound/audio_thread.h \
  general/myio.h \
  general/useful.h \
  general/mystring.h \
  general/settings.h \
  general/bspline.h \
  include/freqpair.h \
  widgets/ledindicator.h \
  widgets/drawwidget.h \
  widgets/viewwidget.h \
  widgets/myscrollbar.h \
  widgets/mainwindow/mainwindow.h \
  widgets/openfiles/openfiles.h \
  widgets/freq/freqdrawwidget.h \
  widgets/freq/freqwidgetGL.h \
  widgets/freq/freqview.h \
  widgets/pitchcompass/pitchcompassview.h \
  widgets/pitchcompass/pitchcompassdrawwidget.h \
  widgets/summary/summarydrawwidget.h \
  widgets/summary/summaryview.h \
  widgets/tuner/tunerview.h \
  widgets/tuner/tunerwidget.h \
  widgets/hblock/hblockview.h \
  widgets/hblock/hblockwidget.h \
  widgets/hstack/hstackview.h \
  widgets/hstack/hstackwidget.h \
  widgets/hbubble/hbubbleview.h \
  widgets/hbubble/hbubblewidget.h \
  widgets/hcircle/hcircleview.h \
  widgets/hcircle/hcirclewidget.h \
  widgets/wave/waveview.h \
  widgets/wave/wavewidget.h \
  global/gdata.h \
  sound/filters/Filter.h \
  sound/filters/IIR_Filter.h \
  sound/filters/FixedAveragingFilter.h \
  sound/filters/GrowingAveragingFilter.h \
  sound/filters/FastSmoothedAveragingFilter.h \
  include/array1d.h \
  include/array2d.h \
  sound/channel.h \
  sound/soundfile.h \
  global/view.h \
  include/myqmutex.h \
  general/mytransforms.h \
  widgets/timeaxis.h \
  sound/analysisdata.h \
  widgets/volumemeter/volumemeterview.h \
  widgets/volumemeter/volumemeterwidget.h \
  include/myassert.h \
  sound/zoomlookup.h \
  dialogs/savedialog.h \
  dialogs/opendialog.h \
  general/equalloudness.h \
  widgets/piano/pianowidget.h \
  widgets/freq/amplitudewidget.h \
  widgets/piano/pianoview.h \
  widgets/htrack/htrackview.h \
  widgets/htrack/htrackwidget.h \
  widgets/htrack/piano3d.h \
  widgets/correlation/correlationview.h \
  widgets/correlation/correlationwidget.h \
  widgets/fft/fftwidget.h \
  widgets/fft/fftview.h \
  widgets/cepstrum/cepstrumwidget.h \
  widgets/cepstrum/cepstrumview.h \
  sound/notedata.h \
  general/large_vector.h \
  widgets/mylabel.h \
  sound/notesynth.h \
  general/fast_smooth.h \
  global/conversions.h \
  widgets/debugview/debugwidget.h \
  widgets/debugview/debugview.h \
  widgets/score/scorewidget.h \
  widgets/score/scoreview.h \
  widgets/score/scoresegmentiterator.h \
  dialogs/tartinisettingsdialog.h \
  widgets/vibrato/vibratowidget.h \
  widgets/vibrato/vibratoview.h \
  widgets/vibrato/vibratotunerwidget.h \
  widgets/vibrato/vibratotimeaxis.h \
  widgets/vibrato/vibratospeedwidget.h \
  widgets/vibrato/vibratoperiodwidget.h \
  widgets/vibrato/vibratocirclewidget.h \
  include/safe_bool.h \
  include/SmartPtr.h \
  general/prony.h \
  general/mymatrix.h \
  general/myglfonts.h \
  general/mygl.h \
  general/myqt.h \
  general/musicnotes.h \
  general/myalgo.h \
  include/RingBuffer.h

SOURCES += main.cpp \
  sound/wave_stream.cpp \
  sound/audio_thread.cpp \
  general/myio.cpp \
  sound/filters/IIR_Filter.cpp \
  sound/filters/FixedAveragingFilter.cpp \
  sound/filters/GrowingAveragingFilter.cpp \
  sound/filters/FastSmoothedAveragingFilter.cpp \
  sound/channel.cpp \
  sound/soundfile.cpp \
  general/useful.cpp \
  general/mystring.cpp \
  general/settings.cpp \
  general/bspline.cpp \
  widgets/ledindicator.cpp \
  widgets/drawwidget.cpp \
  widgets/myscrollbar.cpp \
  widgets/viewwidget.cpp \
  widgets/mainwindow/mainwindow.cpp \
  widgets/openfiles/openfiles.cpp \
  widgets/freq/freqdrawwidget.cpp \
  widgets/freq/freqwidgetGL.cpp \
  widgets/freq/freqview.cpp \
  widgets/pitchcompass/pitchcompassview.cpp \
  widgets/pitchcompass/pitchcompassdrawwidget.cpp \
  widgets/summary/summarydrawwidget.cpp \
  widgets/summary/summaryview.cpp \
  widgets/tuner/tunerview.cpp \
  widgets/tuner/tunerwidget.cpp \
  widgets/hblock/hblockview.cpp \
  widgets/hblock/hblockwidget.cpp \
  widgets/hstack/hstackview.cpp \
  widgets/hstack/hstackwidget.cpp \
  widgets/hbubble/hbubbleview.cpp \
  widgets/hbubble/hbubblewidget.cpp \
  widgets/hcircle/hcircleview.cpp \
  widgets/hcircle/hcirclewidget.cpp \
  widgets/wave/waveview.cpp \
  widgets/wave/wavewidget.cpp \
  global/gdata.cpp \
  cleanup.c \
  global/view.cpp \
  sound/sound_stream.cpp \
  general/mytransforms.cpp \
  widgets/timeaxis.cpp \
  sound/analysisdata.cpp \
  widgets/volumemeter/volumemeterview.cpp \
  widgets/volumemeter/volumemeterwidget.cpp \
  sound/zoomlookup.cpp \
  dialogs/savedialog.cpp \
  dialogs/opendialog.cpp \
  general/equalloudness.cpp \
  widgets/piano/pianowidget.cpp \
  widgets/freq/amplitudewidget.cpp \
  widgets/piano/pianoview.cpp \
  widgets/htrack/htrackview.cpp \
  widgets/htrack/htrackwidget.cpp \
  widgets/htrack/piano3d.cpp \
  widgets/correlation/correlationview.cpp \
  widgets/correlation/correlationwidget.cpp \
  widgets/fft/fftwidget.cpp \
  widgets/fft/fftview.cpp \
  widgets/cepstrum/cepstrumwidget.cpp \
  widgets/cepstrum/cepstrumview.cpp \
  sound/notedata.cpp \
  widgets/mylabel.cpp \
  sound/notesynth.cpp \
  general/fast_smooth.cpp \
  global/conversions.cpp \
  widgets/debugview/debugwidget.cpp \
  widgets/debugview/debugview.cpp \
  widgets/score/scorewidget.cpp \
  widgets/score/scoreview.cpp \
  widgets/score/scoresegmentiterator.cpp \
  dialogs/tartinisettingsdialog.cpp \
  widgets/vibrato/vibratowidget.cpp \
  widgets/vibrato/vibratoview.cpp \
  widgets/vibrato/vibratotunerwidget.cpp \
  widgets/vibrato/vibratotimeaxis.cpp \
  widgets/vibrato/vibratospeedwidget.cpp \
  widgets/vibrato/vibratoperiodwidget.cpp \
  widgets/vibrato/vibratocirclewidget.cpp \
  general/prony.cpp \
  general/mymatrix.cpp \
  general/myglfonts.cpp \
  general/myqt.cpp \
  general/musicnotes.cpp \
  general/myalgo.cpp
 
RESOURCES += pitch.qrc
PRECOMPILED_HEADER = static.h
  
MYPATHS = include/ general/ sound/ widgets/ global/ dialogs/
MYPATHS += widgets/mainwindow widgets/freq widgets/summary widgets/pitchcompass widgets/openfiles widgets/volumemeter widgets/tuner widgets/hblock widgets/hstack widgets/wave widgets/piano widgets/htrack widgets/correlation widgets/fft widgets/cepstrum widgets/hbubble widgets/hcircle widgets/debugview widgets/score widgets/vibrato sound/filters

INCLUDEPATH += $$MYPATHS
DEPENDPATH += $$MYPATHS

unix{
  debug {
    OBJECTS_DIR=.debug_obj
    MOC_DIR=.debug_moc
  }
  release {
    OBJECTS_DIR=.obj
    MOC_DIR=.moc
  }
  macx{

    #INCLUDEPATH += macx/
    #DEPENDPATH += macx/
    #HEADERS += macx/audio_stream.h
    #SOURCES += macx/audio_stream.cpp

    INCLUDEPATH += rtAudio/
    DEPENDPATH += rtAudio/
    HEADERS += rtAudio/audio_stream.h rtAudio/RtAudio.h rtAudio/RtError.h
    SOURCES += rtAudio/audio_stream.cpp rtAudio/rtAudio.cpp

    QMAKE_LFLAGS_SONAME = -W1,-install_name,@executable_path/..Frameworks/
    INCLUDEPATH += $$MY_INCLUDE_PATH
    LIBS += -framework CoreAudio $$MY_LIB_PATH -lfftw3f -lqwt
    # -framework CoreFoundation -framework ApplicationServices -framework CoreData -framework System -framework Carbon -framework CoreServices -framework AppKit -framework AGL -framework Quartz -framework Foundation
    RC_FILE = icons/pitch.icns
    DEFINES += MACX
    DEFINES += __MACOSX_CORE__

    #CONFIG += link_prl
    #CONFIG += static
    #REQUIRES  = full-config
    #QMAKE_CXXFLAGS += -fast -mcpu=7450
    #QMAKE_CXXFLAGS_RELEASE -= -Os
    QMAKE_CXXFLAGS_RELEASE += -O2
    #CONFIG += ppc
  }
  else{ #linux

    #old sound library routines
    #INCLUDEPATH += unix/
    #DEPENDPATH += unix/
    #HEADERS += unix/audio_stream.h
    #SOURCES += unix/audio_stream.cpp

    #RtAudio sound library routines
    INCLUDEPATH += rtAudio/
    DEPENDPATH += rtAudio/
    HEADERS += rtAudio/audio_stream.h rtAudio/RtAudio.h rtAudio/RtError.h
    SOURCES += rtAudio/audio_stream.cpp rtAudio/RtAudio.cpp
    DEFINES += __LINUX_OSS__
    DEFINES += __LINUX_ALSA__
    #DEFINES += __LINUX_JACK__  #Uncomment to use Jack. Note untested.

    DEFINES += LINUX
    INCLUDEPATH += $$MY_INCLUDE_PATH
    LIBS += $$MY_LIB_PATH -lfftw3f -lqwt-qt4 -lasound
    CONFIG += warn_off
    QMAKE_CXXFLAGS += -Wall -Wno-non-virtual-dtor
    QMAKE_CXXFLAGS -= -g
    profile {
      QMAKE_CXXFLAGS += -pg
      QMAKE_LFLAGS += -pg
    }
  }
}
win32{
  INCLUDEPATH += $$MY_INCLUDE_PATH

  #to use windows MM sound libries uncomment the following block
  INCLUDEPATH += "./" windows/
  DEPENDPATH += windows/
  HEADERS += windows/audio_stream.h
  SOURCES += windows/audio_stream.cpp
  LIBS += -lwinmm

  #to use rtAudio uncomment the following block
  #INCLUDEPATH += rtAudio/
  #DEPENDPATH += rtAudio/
  #HEADERS += rtAudio/audio_stream.h rtAudio/RtAudio.h rtAudio/RtError.h
  #SOURCES += rtAudio/audio_stream.cpp
  ##DEFINES += __WINDOWS_DS__   #untested under MinGW
  #DEFINES += __WINDOWS_ASIO__  #tested successfully. Note requires ASIO sound drivers
  #SOURCES += asio/asio.cpp asio/asiodrivers.cpp asio/asiolist.cpp asio/iasiothiscallresolver.cpp

  DEFINES += WINDOWS
  REQUIRES += thread
  LIBS += $$MY_LIB_PATH -lqwt5 -lfftw3f-3 -lole32
  #DEFINES += QT_DLL QT_THREAD_SUPPORT
  DEFINES -= UNICODE       #I think some things broke without this?
}
debug{
  DEFINES += MYDEBUG
}

QT +=  opengl qt3support 
CONFIG += uic

UI_DIR = dialogs

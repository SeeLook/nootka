TARGET = shelltuner
CONFIG -= qt

INCLUDEPATH += ../rtAudio/
INCLUDEPATH += ../tartinilib/
#INCLUDEPATH += /fftw/include    #specify your include path for fftw if needed

#Uncomment the defines for whichever ones you want to support
DEFINES += __LINUX_ALSA__
DEFINES += __LINUX_OSS__
#DEFINES += __LINUX_JACK__    #untested
#DEFINES += __MACOSX_CORE__
#DEFINES += __WINDOWS_ASIO__  #tested successfully. Note requires ASIO sound drivers
#DEFINES += __WINDOWS_DS__    #untested under MinGW

HEADERS += tartini_core.h c_vector.h
SOURCES += \
  main.cpp \
  ../rtAudio/RtAudio.cpp

LIBS += -L../tartinilib -lfftw3f -ltartini -lpthread -lasound

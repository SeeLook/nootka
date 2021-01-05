########################################################################
# CMakeList.txt NootkaSound library
########################################################################

###########################################################################
########   Looking for fftw3
###########################################################################
IF(FFTW3_LIBRARIES AND FFTW3_INCLUDE_DIRS)
  set(FFTW3_FOUND TRUE)
ELSE(FFTW3_LIBRARIES AND FFTW3_INCLUDE_DIRS)
  find_path(FFTW3_INCLUDE_DIR
    NAMES
      fftw3.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
      ${CMAKE_SOURCE_DIR}/fftw
      ${CMAKE_SOURCE_DIR}/../fftw
      ${CMAKE_BINARY_DIR}/fftw
      ${CMAKE_BINARY_DIR}/../fftw
  )
  IF(WIN32)
    find_library(FFTW3_LIBRARY
      NAMES
        libfftw3f-3.dll
      PATHS
        ${CMAKE_SOURCE_DIR}/fftw
        ${CMAKE_SOURCE_DIR}/../fftw
          ${CMAKE_BINARY_DIR}/fftw
        ${CMAKE_BINARY_DIR}/../fftw
    )
  ELSE(WIN32)
    find_library(FFTW3_LIBRARY
      NAMES
        fftw3f
      PATHS
        /usr/lib
        /usr/lib64
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
  )
  ENDIF(WIN32)

  set(FFTW3_INCLUDE_DIRS "${FFTW3_INCLUDE_DIR}" CACHE STRING "FFTW include directory")
  set(FFTW3_LIBRARIES "${FFTW3_LIBRARY}" CACHE STRING "FFTW library path")

  IF(FFTW3_INCLUDE_DIRS AND FFTW3_LIBRARIES)
     set(FFTW3_FOUND TRUE)
  ENDIF(FFTW3_INCLUDE_DIRS AND FFTW3_LIBRARIES)

  IF(FFTW3_FOUND)
      message(STATUS "Fftw3 was Found in:\n${FFTW3_LIBRARIES}\nheaders are in: ${FFTW3_INCLUDE_DIRS}")
  ELSE(FFTW3_FOUND)
      message(FATAL_ERROR "Could not find Fftw3\nTry call cmake with args:\n-DFFTW3_LIBRARIES=/path_to/libfftw3.la(dll) -DFFTW3_INCLUDE_DIRS=/path_to_dir_with/fftw3.h\nor put those files in build dir or source dir")
  ENDIF(FFTW3_FOUND)

ENDIF(FFTW3_LIBRARIES AND FFTW3_INCLUDE_DIRS)

include_directories(${FFTW3_INCLUDE_DIRS})


###########################################################################
########   Looking for Ogg Vorbis (ogg decoder)
###########################################################################
IF(VORBIS_LIBRARIES AND OGG_INCLUDE_DIRS AND VORBIS_INCLUDE_DIRS)
  set(OGG_FOUND TRUE)
ELSE(VORBIS_LIBRARIES AND OGG_INCLUDE_DIRS AND VORBIS_INCLUDE_DIRS)
  find_path(VORBIS_INCLUDE_DIR
    NAMES
      "vorbis/vorbisfile.h"
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
      ${CMAKE_SOURCE_DIR}/../libvorbis/include
      ${CMAKE_SOURCE_DIR}/libvorbis/include
      ${CMAKE_BINARY_DIR}/../libvorbis/include
  )
  find_path(OGG_INCLUDE_DIR
    NAMES
      "ogg/ogg.h"
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
      ${CMAKE_SOURCE_DIR}/libogg/include
      ${CMAKE_SOURCE_DIR}}/../libogg/include
      ${CMAKE_BINARY_DIR}/libogg/include
      ${CMAKE_BINARY_DIR}/../libogg/include
  )
  IF(WIN32)
    find_library(VORBIS_LIBRARY
      NAMES
        libvorbisfile.dll
      PATHS
      ${CMAKE_SOURCE_DIR}/vorbis-tools
      ${CMAKE_SOURCE_DIR}}/../vorbis-tools
      ${CMAKE_BINARY_DIR}/../vorbis-tools
    )
  ELSE(WIN32)
    find_library(VORBIS_LIBRARY
      NAMES
        vorbisfile
      PATHS
        /usr/lib
        /usr/lib64
        /usr/local/lib
        /opt/local/lib
        /sw/lib
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}}/../libvorbis/include
        ${CMAKE_CURRENT_SOURCE_DIR}
  )
  ENDIF(WIN32)

  set(VORBIS_INCLUDE_DIRS    ${VORBIS_INCLUDE_DIR} )
  set(OGG_INCLUDE_DIRS       ${OGG_INCLUDE_DIR})
  set(VORBIS_LIBRARIES       ${VORBIS_LIBRARY} CACHE STRING "Ogg vorbis library path")

  IF(VORBIS_INCLUDE_DIRS AND OGG_INCLUDE_DIRS AND VORBIS_LIBRARIES)
     set(OGG_FOUND TRUE)
  ENDIF(VORBIS_INCLUDE_DIRS AND OGG_INCLUDE_DIRS AND VORBIS_LIBRARIES)

  IF(OGG_FOUND)
      message(STATUS "Ogg Vorbis was Found in:\n${VORBIS_LIBRARIES}\nheaders are in: ${OGG_INCLUDE_DIRS} ${VORBIS_INCLUDE_DIRS}")
  ELSE(OGG_FOUND)
      message(FATAL_ERROR "Could not find libvorbis\nTry call cmake with args:\n-DVORBIS_LIBRARIES =/path_to/libvorbisfile.so(dll) -DVORBIS_INCLUDE_DIRS=/path_to_dir_with/vorbis/vorbisfile.h\n-DOGG_INCLUDE_DIRS=/path_to_dir_with/ogg/ogg.h\nor put those files in build dir or source dir")
  ENDIF(OGG_FOUND)

ENDIF(VORBIS_LIBRARIES AND OGG_INCLUDE_DIRS AND VORBIS_INCLUDE_DIRS)

include_directories(${OGG_INCLUDE_DIRS})
include_directories(${VORBIS_INCLUDE_DIR})

###########################################################################
########   Looking for SoundTouch
###########################################################################
if (UNIX)
  IF(SOUNDTOUCH_LIBRARIES AND SOUNDTOUCH_INCLUDE_DIRS)
    set(SOUNDTOUCH_FOUND TRUE)
  ELSE(SOUNDTOUCH_LIBRARIES AND SOUNDTOUCH_INCLUDE_DIRS)
    find_path(SOUNDTOUCH_INCLUDE_DIR
          NAMES
            soundtouch/SoundTouch.h
          PATHS
            /usr/include
            /usr/local/include
            /opt/local/include
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_BINARY_DIR}/../soundtouch
        )

    find_library(SOUNDTOUCH_LIBRARY
          NAMES
            SoundTouch
          PATHS
            /usr/lib
            /usr/lib64
            /usr/local/lib
            /opt/local/lib
            /sw/lib
            ${CMAKE_BINARY_DIR}/../soundtouch
            ${CMAKE_CURRENT_SOURCE_DIR}
    )

    set(SOUNDTOUCH_INCLUDE_DIRS
      ${SOUNDTOUCH_INCLUDE_DIR}
    )
    set(SOUNDTOUCH_LIBRARIES
      ${SOUNDTOUCH_LIBRARY}
    )

    IF(SOUNDTOUCH_INCLUDE_DIRS AND SOUNDTOUCH_LIBRARIES)
      set(SOUNDTOUCH_FOUND TRUE)
    ENDIF(SOUNDTOUCH_INCLUDE_DIRS AND SOUNDTOUCH_LIBRARIES)

    IF(SOUNDTOUCH_FOUND)
        message(STATUS "SoundTouch was Found in:\n${SOUNDTOUCH_LIBRARIES}\nheaders are in: ${SOUNDTOUCH_INCLUDE_DIRS}")
    ELSE(SOUNDTOUCH_FOUND)
        message(FATAL_ERROR "Could not find SoundTouch\nTry call cmake with args:\n-DSOUNDTOUCH_LIBRARIES=/path_to/libSoundTouch.la(dll) -DSOUNDTOUCH_INCLUDE_DIRS=/path_to_dir_with/libSoundTouch.h\nor put those files in build dir or source dir")
    ENDIF(SOUNDTOUCH_FOUND)

  ENDIF(SOUNDTOUCH_LIBRARIES AND SOUNDTOUCH_INCLUDE_DIRS)
endif (UNIX)

###########################################################################
#libraries and compiler definitions for RtMidi
###########################################################################
if (APPLE)
      find_library(CoreAudio_LIB             NAMES CoreAudio)
      find_library(CoreMidi_LIB              NAMES CoreMIDI)
      find_library(CoreFoundation_LIB        NAMES CoreFoundation)
endif (APPLE)

IF(UNIX AND NOT APPLE)
  find_library(ALSA_LIBRARY
      NAMES
      asound
      PATHS
      /usr/lib
      /usr/lib/x86_64-linux-gnu
      /usr/local/lib
      /opt/local/lib
  )
  find_library(PTHREAD_LIBRARY
      NAMES
      pthread
      PATHS
      /usr/lib
      /usr/lib/x86_64-linux-gnu
      /usr/local/lib
      /opt/local/lib
  )
  mark_as_advanced(ALSA_LIBRARY PTHREAD_LIBRARY)

ENDIF(UNIX AND NOT APPLE)

IF(WIN32)
#   add_definitions(-D__WINDOWS_WASAPI__)
#   add_definitions(-D__WINDOWS_DS__)
#   add_definitions(-D__WINDOWS_ASIO__)
#   add_definitions(-D__WINDOWS_MM__)
#   set(WIN_LIBS winmm ole32 dsound uuid ksuser) # uuid & ksuser are required by WASAPI
ENDIF(WIN32)

if(APPLE)
  add_definitions(-D__MACOSX_CORE__)
endif(APPLE)


IF(UNIX AND NOT APPLE)
  ##### PulseAudio ###########################################
      find_path(PULSE_INCLUDE_DIR
        NAMES
          pulseaudio.h
        PATHS
          /usr/include
          /usr/include/pulse
          /usr/local/include
          /opt/local/include
          /sw/include

      )
      if (PULSE_INCLUDE_DIR)
          find_library(PULSESIMPLE_LIBRARY
              NAMES
              pulse-simple
              PATHS
              /usr/lib
              /usr/lib/x86_64-linux-gnu
              /usr/local/lib
              /opt/local/lib
          )
          find_library(PULSEERROR_LIBRARY
              NAMES
              pulse
              PATHS
              /usr/lib
              /usr/lib/x86_64-linux-gnu
              /usr/local/lib
              /opt/local/lib
          )
      endif (PULSE_INCLUDE_DIR)
    if (PULSE_INCLUDE_DIR)
        option(ENABLE_PULSEAUDIO "Enable support for PulseAudio" ON)
    else(PULSE_INCLUDE_DIR)
        option(ENABLE_PULSEAUDIO "Enable support for PulseAudio" OFF)
        set(PULSEERROR_LIBRARY_FOUND TRUE)
        set(PULSESIMPLE_LIBRARY_FOUND TRUE)
    endif (PULSE_INCLUDE_DIR)


  ##### JACK - Jack Audio Connection Kit ##############
      find_path(JACK_INCLUDE_DIR
        NAMES
          jack.h
        PATHS
          /usr/include
          /usr/include/jack
          /usr/local/include
          /opt/local/include
          /sw/include

      )
      if (JACK_INCLUDE_DIR)
          find_library(JACK_LIBRARY
              NAMES
              jack
              PATHS
              /usr/lib
              /usr/lib/x86_64-linux-gnu
              /usr/local/lib
              /opt/local/lib
          )
      endif (JACK_INCLUDE_DIR)

    if (JACK_INCLUDE_DIR)
        option(ENABLE_JACK "Enable support for JACK (Jack Audio Connection Kit)" ON)
    else (JACK_INCLUDE_DIR)
        option(ENABLE_JACK "Enable support for JACK (Jack Audio Connection Kit)" OFF)
        set(JACK_LIBRARY_FOUND TRUE)
    endif (JACK_INCLUDE_DIR)

  if(ALSA_LIBRARY)
    add_definitions(-D__LINUX_ALSA__)
    message(STATUS "support for ALSA\t\t TRUE")
  else(ALSA_LIBRARY)
    message(STATUS "support for ALSA\t\t FALSE")
  endif(ALSA_LIBRARY)
  if(ENABLE_JACK AND JACK_INCLUDE_DIR)
    add_definitions(-D__UNIX_JACK__)
    include_directories(${JACK_INCLUDE_DIR})
    message(STATUS "support for JACK\t\t TRUE")
  else(ENABLE_JACK AND JACK_INCLUDE_DIR)
    message(STATUS "support for JACK\t\t FALSE")
  endif(ENABLE_JACK AND JACK_INCLUDE_DIR)
  if(ENABLE_PULSEAUDIO AND PULSE_INCLUDE_DIR)
    add_definitions(-D__LINUX_PULSE__)
    include_directories(${PULSE_INCLUDE_DIR})
    message(STATUS "support for PulseAudio\t TRUE")
  else(ENABLE_PULSEAUDIO AND PULSE_INCLUDE_DIR)
    message(STATUS "support for PulseAudio\t FALSE")
  endif(ENABLE_PULSEAUDIO AND PULSE_INCLUDE_DIR)

endif(UNIX AND NOT APPLE)


add_definitions(-DNOOTKASOUND_LIBRARY)

include_directories( . ../core )

if (WIN32)
  include_directories( rt )
endif(WIN32)


set(LIB_NOOTKASOUND_SRC
  tabstractplayer.cpp
  tartiniparams.h
  tmidiout.cpp
  toggscale.cpp
  tpitchfinder.cpp
  taudioobject.h
  trtaudio.cpp
  tcommonlistener.cpp
  trtaudioin.cpp
  trtaudioout.cpp
  tsound.cpp
  ttickcolors.cpp
  tonsetlogic.cpp

  tartini/analysisdata.cpp
  tartini/array1d.h
  tartini/bspline.cpp
  tartini/channel.cpp
  tartini/conversions.cpp
  tartini/fast_smooth.cpp
  tartini/large_vector.h
  tartini/mytransforms.cpp
  tartini/notedata.cpp
  tartini/SmartPtr.h
  tartini/useful.cpp
  tartini/filters/FastSmoothedAveragingFilter.cpp
  tartini/filters/Filter.h
  tartini/filters/FixedAveragingFilter.cpp
  tartini/filters/GrowingAveragingFilter.cpp
  tartini/filters/IIR_Filter.cpp
)

IF(WIN32)
  add_subdirectory( rt )
  link_directories( ${CMAKE_BINARY_DIR}/src )
  set(WIN_LIBS RtAudio)

  list(APPEND LIB_NOOTKASOUND_SRC tasioemitter.h )
  list(APPEND LIB_NOOTKASOUND_SRC
    stouch/SoundTouch.cpp
    stouch/TDStretch.cpp
    stouch/RateTransposer.cpp
    stouch/AAFilter.cpp
    stouch/FIRFilter.cpp
    stouch/FIFOSampleBuffer.cpp
    stouch/cpu_detect_x86.cpp
    stouch/sse_optimized.cpp
    stouch/InterpolateCubic.cpp
    stouch/InterpolateLinear.cpp
    stouch/InterpolateShannon.cpp
  )
else (WIN32)
  list(APPEND LIB_NOOTKASOUND_SRC
    rt/RtAudio.cpp
    rt/RtMidi.cpp
  )
endif(WIN32)

if (WITH_STATIC_LIB)
  add_library(NootkaSound STATIC ${LIB_NOOTKASOUND_SRC} )
else ()
  add_library(NootkaSound SHARED ${LIB_NOOTKASOUND_SRC} )
endif ()
target_link_libraries(NootkaSound
    Qt5::Core
    Qt5::Qml
    NootkaCore
    ${WIN_LIBS}
    ${FFTW3_LIBRARIES}
    ${ALSA_LIBRARY} ${PTHREAD_LIBRARY}
    ${PULSESIMPLE_LIBRARY} ${PULSEERROR_LIBRARY}
    ${JACK_LIBRARY}
    ${VORBIS_LIBRARIES}
    ${SOUNDTOUCH_LIBRARY}
    ${CoreAudio_LIB}
    ${CoreMidi_LIB}
    ${CoreFoundation_LIB}
)

if (NOT WITH_STATIC_LIB)
  if(UNIX AND NOT APPLE)   # Linux path for Nootka binary
    install(TARGETS NootkaSound DESTINATION lib/nootka)
  else(UNIX AND NOT APPLE)
    if(WIN32)       # Windows
      install(TARGETS NootkaSound DESTINATION .)
      install(FILES ${FFTW3_LIBRARIES} DESTINATION .)
      install(FILES ${VORBIS_LIBRARIES} DESTINATION .)
      get_filename_component(VORBIS_DIR "${VORBIS_LIBRARIES}" DIRECTORY)
      install(FILES ${VORBIS_DIR}/libogg.dll ${VORBIS_DIR}/libvorbis.dll DESTINATION .)
    else(WIN32)       # MacOs
      install(TARGETS NootkaSound DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks")
    endif(WIN32)
  endif(UNIX AND NOT APPLE)
endif (NOT WITH_STATIC_LIB)



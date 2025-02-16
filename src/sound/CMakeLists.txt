########################################################################
# CMakeList.txt NootkaSound library
########################################################################

if(MINGW)
    set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
endif()

if (ANDROID)
    # arm64-v8a, armeabi-v7a, armeabi-v6, armeabi, mips, mips64, x86, x86_64
    if (CMAKE_ANDROID_ARCH_ABI MATCHES "arm64")
        set(ARCH_EXT arm64)
    elseif(CMAKE_ANDROID_ARCH_ABI MATCHES "armeabi-v7a")
        set(ARCH_EXT armv7)
    else() # x86, x86_64
        set(ARCH_EXT ${CMAKE_ANDROID_ARCH_ABI})
    endif()
endif()
###########################################################################
########   Looking for fftw3
###########################################################################
if (ANDROID)
    set(FFTW3_LIBRARIES "${CMAKE_CURRENT_SOURCE_DIR}/fftw/${ARCH_EXT}/libfftw3f.a")
    set(FFTW3_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/fftw/")
    message(STATUS "Fftw3 was Found in:\n${FFTW3_LIBRARIES}\nheaders are in: ${FFTW3_INCLUDE_DIRS}")
endif()
if(FFTW3_LIBRARIES AND FFTW3_INCLUDE_DIRS)
    set(FFTW3_FOUND TRUE)
else()
    find_path(FFTW3_INCLUDE_DIR
        NAMES
            "fftw3.h"
        PATHS
            /usr/include
            /usr/local/include
            /opt/local/include
            /sw/include
            ${CMAKE_SOURCE_DIR}/fftw/
            ${CMAKE_SOURCE_DIR}/../fftw/
            ${CMAKE_BINARY_DIR}/fftw/
            ${CMAKE_BINARY_DIR}/../fftw/
            /usr/x86_64-w64-mingw32/include/
    )
    find_library(FFTW3_LIBRARY
        NAMES
            fftw3f libfftw3f-3
        PATHS
            /usr/lib
            /usr/lib64
            /usr/local/lib
            /opt/local/lib
            /sw/lib
            ${CMAKE_SOURCE_DIR}/fftw/
            ${CMAKE_SOURCE_DIR}/../fftw/
            ${CMAKE_BINARY_DIR}/fftw/
            ${CMAKE_BINARY_DIR}/../fftw/
    )

    set(FFTW3_INCLUDE_DIRS "${FFTW3_INCLUDE_DIR}" CACHE STRING "FFTW include directory")
    set(FFTW3_LIBRARIES "${FFTW3_LIBRARY}" CACHE STRING "FFTW library path")

    if(FFTW3_INCLUDE_DIRS AND FFTW3_LIBRARIES)
        set(FFTW3_FOUND TRUE)
    endif()

    if(FFTW3_FOUND)
        message(STATUS "Fftw3 was Found in:\n${FFTW3_LIBRARIES}\nheaders are in: ${FFTW3_INCLUDE_DIRS}")
    else()
        message(FATAL_ERROR "Could not find Fftw3\nTry call cmake with args:\n-DFFTW3_LIBRARIES=/path_to/libfftw3.la(dll) -DFFTW3_INCLUDE_DIRS=/path_to_dir_with/fftw3.h\nor put those files in build dir or source dir")
    endif()
endif()

target_include_directories(nootka PRIVATE ${FFTW3_INCLUDE_DIRS})

###########################################################################
########   Looking for Ogg Vorbis (ogg decoder)
###########################################################################
if(ANDROID)
    set(VORBIS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
    set(OGG_INCLUDE_DIR  ${CMAKE_CURRENT_SOURCE_DIR})
    # On Android all vorbis/file/ogg are packed inside single static lib
    set(OGG_LIB "${CMAKE_CURRENT_SOURCE_DIR}/ogg/${ARCH_EXT}/liboggstatic.a")
else()
    if(VORBIS_LIBRARIES AND OGG_INCLUDE_DIRS AND VORBIS_INCLUDE_DIRS)
        set(OGG_FOUND TRUE)
    else()
        find_path(VORBIS_INCLUDE_DIR
            NAMES
                "vorbis/vorbisfile.h"
            PATHS
                /usr/include
                /usr/local/include
                /opt/local/include
                /sw/include
                ${CMAKE_SOURCE_DIR}/../libvorbis/include/
                ${CMAKE_SOURCE_DIR}/libvorbis/include/
                ${CMAKE_BINARY_DIR}/../libvorbis/include/
                ${CMAKE_BINARY_DIR}/libvorbis/include/
                /usr/x86_64-w64-mingw32/include/
        )
        find_path(OGG_INCLUDE_DIR
            NAMES
                "ogg/ogg.h"
            PATHS
                /usr/include
                /usr/local/include
                /opt/local/include
                /sw/include
                ${CMAKE_SOURCE_DIR}/libogg/include/
                ${CMAKE_SOURCE_DIR}}/../libogg/include/
                ${CMAKE_BINARY_DIR}/libogg/include/
                ${CMAKE_BINARY_DIR}/../libogg/include/
                /usr/x86_64-w64-mingw32/include/
        )
        get_filename_component(VORBIS_DIR "${VORBIS_LIBRARIES}" DIRECTORY)
        if (MINGW)
            set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")
        endif()
        message(STATUS "\n${VORBIS_DIR}")
        find_library(VORBISFILE_LIB
            NAMES
                vorbisfile libvorbisfile
            PATHS
                ${VORBIS_DIR}
                /usr/lib
                /usr/lib64
                /usr/local/lib
                /opt/local/lib
                /sw/lib
                ${CMAKE_BINARY_DIR}}/libvorbis/
                ${CMAKE_BINARY_DIR}/../libvorbis/
                ${CMAKE_BINARY_DIR}}/../vorbis-tools/
                ${CMAKE_BINARY_DIR}/vorbis-tools/
        )
        find_library(OGG_LIB
            NAMES
                ogg libogg
            PATHS
                ${VORBIS_DIR}
                /usr/lib
                /usr/lib64
                /usr/local/lib
                /opt/local/lib
                /sw/lib
                ${CMAKE_BINARY_DIR}}/libvorbis/
                ${CMAKE_BINARY_DIR}/../libvorbis/
                ${CMAKE_BINARY_DIR}}/vorbis-tools/
                ${CMAKE_BINARY_DIR}/../vorbis-tools/
        )
        find_library(VORBIS_LIB
            NAMES
                vorbis libvorbis
            PATHS
                ${VORBIS_DIR}
                /usr/lib
                /usr/lib64
                /usr/local/lib
                /opt/local/lib
                /sw/lib
                ${CMAKE_BINARY_DIR}}/libvorbis/
                ${CMAKE_BINARY_DIR}/../libvorbis/
                ${CMAKE_BINARY_DIR}}/vorbis-tools/
                ${CMAKE_BINARY_DIR}/../vorbis-tools/
        )

        message(STATUS "\n${VORBIS_LIB}\n${OGG_LIB}\n${VORBISFILE_LIB}")
        set(VORBIS_INCLUDE_DIRS ${VORBIS_INCLUDE_DIR} )
        set(OGG_INCLUDE_DIRS ${OGG_INCLUDE_DIR})
        set(VORBIS_LIBRARIES ${VORBISFILE_LIB} CACHE STRING "Ogg vorbis library path")

        if(VORBIS_INCLUDE_DIRS AND OGG_INCLUDE_DIRS AND VORBIS_LIBRARIES)
            set(OGG_FOUND TRUE)
        endif()

        if(OGG_FOUND)
            message(STATUS "Ogg Vorbis was Found in:\n${VORBIS_LIBRARIES}\nheaders are in: ${OGG_INCLUDE_DIRS} ${VORBIS_INCLUDE_DIRS}")
        else()
            message(FATAL_ERROR "Could not find libvorbis\nTry call cmake with args:\n-DVORBIS_LIBRARIES =/path_to/libvorbisfile.so(dll) -DVORBIS_INCLUDE_DIRS=/path_to_dir_with/vorbis/vorbisfile.h\n-DOGG_INCLUDE_DIRS=/path_to_dir_with/ogg/ogg.h\nor put those files in build dir or source dir")
        endif()
    endif()
endif()

include_directories(${OGG_INCLUDE_DIRS})
include_directories(${VORBIS_INCLUDE_DIR})

###########################################################################
########   Looking for SoundTouch
###########################################################################
if(UNIX AND NOT ANDROID) # Linux and MacOs
    if(SOUNDTOUCH_LIBRARIES AND SOUNDTOUCH_INCLUDE_DIRS)
        set(SOUNDTOUCH_FOUND TRUE)
    else()
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

        set(SOUNDTOUCH_INCLUDE_DIRS ${SOUNDTOUCH_INCLUDE_DIR})
        set(SOUNDTOUCH_LIBRARIES ${SOUNDTOUCH_LIBRARY})

        if(SOUNDTOUCH_INCLUDE_DIRS AND SOUNDTOUCH_LIBRARIES)
            set(SOUNDTOUCH_FOUND TRUE)
        endif()

        if(SOUNDTOUCH_FOUND)
            message(STATUS "SoundTouch was Found in:\n${SOUNDTOUCH_LIBRARIES}\nheaders are in: ${SOUNDTOUCH_INCLUDE_DIRS}")
        else()
            message(FATAL_ERROR "Could not find SoundTouch\nTry call cmake with args:
                -DSOUNDTOUCH_LIBRARIES=/path_to/libSoundTouch.la(dll) -DSOUNDTOUCH_INCLUDE_DIRS=/path_to_dir_with/libSoundTouch.h
                or put those files in build dir or source dir"
            )
        endiF()
    endif()
endif()

###########################################################################
#libraries and compiler definitions for RtMidi
###########################################################################
if(APPLE)
    find_library(CoreAudio_LIB NAMES CoreAudio)
    find_library(CoreMidi_LIB NAMES CoreMIDI)
    find_library(CoreFoundation_LIB NAMES CoreFoundation)
endif (APPLE)

if(UNIX AND NOT APPLE AND NOT ANDROID)
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
endif()

if(APPLE)
    target_compile_definitions(nootka PUBLIC
        -D__MACOSX_CORE__
    )
endif()


if(UNIX AND NOT APPLE AND NOT ANDROID)
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
    endif()
    if (PULSE_INCLUDE_DIR)
        option(ENABLE_PULSEAUDIO "Enable support for PulseAudio" ON)
    else(PULSE_INCLUDE_DIR)
        option(ENABLE_PULSEAUDIO "Enable support for PulseAudio" OFF)
        set(PULSEERROR_LIBRARY_FOUND TRUE)
        set(PULSESIMPLE_LIBRARY_FOUND TRUE)
    endif(PULSE_INCLUDE_DIR)

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
    endif()

    if(JACK_INCLUDE_DIR)
        option(ENABLE_JACK "Enable support for JACK (Jack Audio Connection Kit)" ON)
    else ()
        option(ENABLE_JACK "Enable support for JACK (Jack Audio Connection Kit)" OFF)
        set(JACK_LIBRARY_FOUND TRUE)
    endif()

    if(ALSA_LIBRARY)
        target_compile_definitions(nootka PUBLIC
            -D__LINUX_ALSA__
        )
        message(STATUS "support for ALSA\t\t TRUE")
    else()
        message(STATUS "support for ALSA\t\t FALSE")
    endif()

    if(ENABLE_JACK AND JACK_INCLUDE_DIR)
        target_compile_definitions(nootka PUBLIC
            -D__UNIX_JACK__
        )
        include_directories(${JACK_INCLUDE_DIR})
        message(STATUS "support for JACK\t\t TRUE")
    else()
        message(STATUS "support for JACK\t\t FALSE")
    endif()

    if(ENABLE_PULSEAUDIO AND PULSE_INCLUDE_DIR)
        target_compile_definitions(nootka PUBLIC
            -D__LINUX_PULSE__
        )
        include_directories(${PULSE_INCLUDE_DIR})
        message(STATUS "support for PulseAudio\t TRUE")
    else()
        message(STATUS "support for PulseAudio\t FALSE")
    endif()
endif()

add_definitions(-DNOOTKASOUND_LIBRARY)

if (WIN32)
    include_directories(rt)
endif(WIN32)


set(LIB_NOOTKASOUND_SRC
    tabstractplayer.cpp
    tartiniparams.h
    # tmidiout.cpp
    toggscale.cpp
    tpitchfinder.cpp
    taudioobject.h
    # trtaudio.cpp
    tcommonlistener.cpp
    # trtaudioin.cpp
    # trtaudioout.cpp
    tsound.cpp
    ttickcolors.cpp
    tonsetlogic.cpp
    tnotesbaritem.cpp

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

if(WIN32)
    add_subdirectory(rt)
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
else()
    if(ANDROID)
        list(APPEND LIB_NOOTKASOUND_SRC
            tqtaudioin.cpp
            tqtaudioout.cpp
            taudiobuffer.h

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
    else()
        list(APPEND LIB_NOOTKASOUND_SRC
            trtaudio.cpp
            trtaudioin.cpp
            trtaudioout.cpp
            tmidiout.cpp

            rt/RtAudio.cpp
            rt/RtMidi.cpp
        )
    endif()
endif()

target_sources(nootka PUBLIC
    ${LIB_NOOTKASOUND_SRC}
)

target_link_libraries(nootka PUBLIC
    ${WIN_LIBS}
    ${FFTW3_LIBRARIES}
    ${VORBISFILE_LIB} ${VORBIS_LIB} ${OGG_LIB}
    ${ALSA_LIBRARY} ${PTHREAD_LIBRARY}
    ${PULSESIMPLE_LIBRARY} ${PULSEERROR_LIBRARY}
    ${JACK_LIBRARY}
    ${SOUNDTOUCH_LIBRARY}
    ${CoreAudio_LIB}
    ${CoreMidi_LIB}
    ${CoreFoundation_LIB}
)

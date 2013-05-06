project(nootka)
cmake_minimum_required(VERSION 2.8)
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE release)
endif (NOT CMAKE_BUILD_TYPE)
find_package(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )
add_definitions(${QT_DEFINITIONS})


include_directories(${QT_INCLUDES})

SET(Nootka_VERSION_MAJOR  "0")
SET(Nootka_VERSION_MINOR  "8")
SET(Nootka_VERSION_PATCH  "80-alpha")

SET(Nootka_VERSION       "${Nootka_VERSION_MAJOR}.${Nootka_VERSION_MINOR}")
SET(Nootka_VERSION_FULL  "${Nootka_VERSION}.${Nootka_VERSION_PATCH}")

###########################################################################
########   Detecting architecture
###########################################################################
if(WIN32)
  set(PACKAGE_ARCHITECTURE x86)
endif(WIN32)
if(APPLE)
  set(PACKAGE_ARCHITECTURE x86_64)
endif(APPLE)

if(NOT PACKAGE_ARCHITECTURE)
   find_program(DPKG_CMD dpkg)
   if(NOT DPKG_CMD)
      find_program(RPM_CMD rpm)
      if(NOT RPM_CMD)
#         message(STATUS "Can not find any program to determine architecture, default to i386.")
#         set(PACKAGE_ARCHITECTURE i386)
        execute_process(COMMAND uname -m
          OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
          OUTPUT_STRIP_TRAILING_WHITESPACE
        )
      else(NOT RPM_CMD)
        execute_process(COMMAND "${RPM_CMD}" --eval %_target_cpu
          OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
          OUTPUT_STRIP_TRAILING_WHITESPACE 
        )
        set(RPM_CMD_FOUND TRUE)
      endif(NOT RPM_CMD)
   else(NOT DPKG_CMD)
      execute_process(COMMAND "${DPKG_CMD}" --print-architecture
        OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
        OUTPUT_STRIP_TRAILING_WHITESPACE
      )
      message(STATUS "-D_FORTIFY_SOURCE=2")
      add_definitions(-D_FORTIFY_SOURCE=2) # for lintian DEB_BUILD_HARDENING_FORTIFY
   endif(NOT DPKG_CMD)
endif(NOT PACKAGE_ARCHITECTURE)
message(STATUS "Architecture is ${PACKAGE_ARCHITECTURE}")
if (NOT DPKG_CMD)
    unset(DPKG_CMD)
endif (NOT DPKG_CMD)

# if(NOT PACKAGE_ARCHITECTURE AND NOT ARCH_CMD)
#    find_program(ARCH_CMD dpkg) # Debian/Ubuntu and derivatives
#    if(ARCH_CMD)
#       execute_process(COMMAND "${ARCH_CMD}" --print-architecture
#         OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
#         OUTPUT_STRIP_TRAILING_WHITESPACE
#       )
#       add_definitions(-D_FORTIFY_SOURCE=2) # for lintian DEB_BUILD_HARDENING_FORTIFY
#       set (ISDEB TRUE)
#    endif(ARCH_CMD)
#    
#    if(NOT ARCH_CMD) # Mandriva/Suse and others RPM-based
#      set (ISDEB FALSE)
# #      if(ARCH_CMD)
#       find_program(ARCH_CMD rpm)
#       execute_process(COMMAND "${ARCH_CMD}" --eval %_target_cpu
#           OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
#           OUTPUT_STRIP_TRAILING_WHITESPACE
#         )
# #       endif(ARCH_CMD)
#    endif(NOT ARCH_CMD)
#    
#    if(NOT ARCH_CMD) # Arch Linux, Gentoo and others
#         find_program(ARCH_CMD uname)
#         execute_process(COMMAND uname -m
#           OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
#           OUTPUT_STRIP_TRAILING_WHITESPACE
#         )
#    endif(NOT ARCH_CMD)   
# endif(NOT PACKAGE_ARCHITECTURE AND NOT ARCH_CMD)
# message(STATUS "Architecture is ${PACKAGE_ARCHITECTURE}")

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
      "${CMAKE_CURRENT_SOURCE_DIR}/fftw"
      "${CMAKE_CURRENT_BINARY_DIR}/fftw"
  )
  IF(WIN32)
    find_library(FFTW3_LIBRARY
      NAMES
        fftw3f.dll
      PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
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

  set(FFTW3_INCLUDE_DIRS
    ${FFTW3_INCLUDE_DIR}
  )
  set(FFTW3_LIBRARIES
    ${FFTW3_LIBRARY}
)

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

# message("Qt is in: ${QT_BINARY_DIR}")

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
      "${CMAKE_CURRENT_SOURCE_DIR}"
      "${CMAKE_CURRENT_BINARY_DIR}"
  )
  find_path(OGG_INCLUDE_DIR
    NAMES
      "ogg/ogg.h"
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
      "${CMAKE_CURRENT_SOURCE_DIR}"
      "${CMAKE_CURRENT_BINARY_DIR}"
  )
  IF(WIN32)
    find_library(VORBIS_LIBRARY
      NAMES
        vorbisfile.dll
      PATHS
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
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
        ${CMAKE_CURRENT_SOURCE_DIR}
  )
  ENDIF(WIN32)

  set(VORBIS_INCLUDE_DIRS    ${VORBIS_INCLUDE_DIR} )
  set(OGG_INCLUDE_DIRS       ${OGG_INCLUDE_DIR})
  set(VORBIS_LIBRARIES       ${VORBIS_LIBRARY}) 

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
## Only under Linux, other will have compiled-in
###########################################################################
if (UNIX AND NOT APPLE)
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
            "${CMAKE_CURRENT_SOURCE_DIR}"
            "${CMAKE_CURRENT_BINARY_DIR}"
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
            ${CMAKE_CURRENT_BINARY_DIR}
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
endif (UNIX AND NOT APPLE)

###########################################################################
#libraries and compiller definitions for RtMidi
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

  add_definitions(-D__LINUX_ALSASEQ__)
ENDIF(UNIX AND NOT APPLE)

IF(WIN32)
  add_definitions(-D__WINDOWS_MM__)
  add_definitions(-D__WINDOWS_DS__)
  add_definitions(-D__WINDOWS_ASIO__)
ENDIF(WIN32)

if(APPLE)
  add_definitions(-D__MACOSX_CORE__)
endif(APPLE)

if (CMAKE_BUILD_TYPE MATCHES 'release')
# set (CMAKE_CXX_FLAGS_RELEASE "-DQT_NO_DEBUG")
  add_definitions(-DQT_NO_DEBUG)
endif (CMAKE_BUILD_TYPE MATCHES 'release')

add_subdirectory(src)


###########################################################################
#INSTALLATION
###########################################################################
if(WIN32)
    set(INS_PREF ".")
else(WIN32)
  if (APPLE)
    set(INS_PREF "nootka.app/Contents/Resources")
  else(APPLE)
    set(INS_PREF "share/nootka")
  endif(APPLE)
endif(WIN32)


if(APPLE)
  INSTALL( CODE"
      execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/packaging/mac/install.sh ${CMAKE_BINARY_DIR} ${QT_BINARY_DIR} ${FFTW3_LIBRARIES} ${VORBIS_LIBRARIES} )
  " )
	INSTALL(FILES "${CMAKE_SOURCE_DIR}/picts/nootka.icns" DESTINATION "${INS_PREF}") # icns for mac
	INSTALL(FILES "${CMAKE_SOURCE_DIR}/picts/nootka-levels.icns" DESTINATION "${INS_PREF}")
	INSTALL(FILES "${CMAKE_SOURCE_DIR}/picts/nootka-exam.icns" DESTINATION "${INS_PREF}")
endif(APPLE)

INSTALL(FILES gpl DESTINATION "${INS_PREF}")
INSTALL(FILES changes DESTINATION "${INS_PREF}")
FILE(GLOB ICONS_PNG "${CMAKE_CURRENT_SOURCE_DIR}/picts/*.png") #png-s for all
INSTALL(FILES ${ICONS_PNG} DESTINATION "${INS_PREF}/picts")
INSTALL(DIRECTORY fonts DESTINATION "${INS_PREF}")
FILE(GLOB langs "${CMAKE_CURRENT_SOURCE_DIR}/lang/*.qm")
if(APPLE OR WIN32)
  LIST(APPEND langs "${QT_TRANSLATIONS_DIR}/qt_cs.qm" "${QT_TRANSLATIONS_DIR}/qt_fr.qm" "${QT_TRANSLATIONS_DIR}/qt_pl.qm" )
endif(APPLE OR WIN32)
INSTALL(FILES ${langs} DESTINATION "${INS_PREF}/lang")
FILE(GLOB snds "${CMAKE_CURRENT_SOURCE_DIR}/sounds/classical-guitar.ogg")
INSTALL(FILES ${snds} DESTINATION "${INS_PREF}/sounds")

if(NOT UNIX) # Windows icons
  INSTALL(FILES picts/nootka-exam.ico picts/nootka.ico picts/nootka-icon.rc picts/nootka-levels.ico picts/pack.ico picts/nootka.rc DESTINATION "${INS_PREF}/picts") #ico for windows
  INSTALL(FILES packaging/nsis/nootka-1250.nsi packaging/nsis/fileasoc.nsh DESTINATION "${INS_PREF}" )
endif(NOT UNIX)

# files related with Linux (mime, desktop entry, icons)
if(UNIX AND NOT APPLE)
    INSTALL(FILES mime/nootka.desktop DESTINATION share/applications)
    INSTALL(FILES picts/nootka.svg DESTINATION "${INS_PREF}/picts")
    INSTALL(FILES picts/nootka.svg picts/levelCreator.png picts/nootka-exam.png DESTINATION share/pixmaps)
    INSTALL( CODE"
      execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/mime )
      execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/mime/install-mime.sh ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}/mime ${CMAKE_CURRENT_SOURCE_DIR}/mime)" )
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/mime/nootka.xml DESTINATION share/mime/packages)
	INSTALL( CODE"
      execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/doc )
      execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/packaging/make-chlog.sh ${Nootka_VERSION_FULL} ${CMAKE_CURRENT_BINARY_DIR}/doc/changelog ${CMAKE_CURRENT_SOURCE_DIR})
	" )
  if (DPKG_CMD) # Debian changelog
      message(STATUS "Debian changelog OK")
      INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/doc/changelog.Debian.gz" DESTINATION share/doc/nootka)
  endif (DPKG_CMD)
	INSTALL(FILES copyright DESTINATION share/doc/nootka)
	INSTALL(FILES packaging/nootka.1.gz DESTINATION share/man/man1)
	INSTALL(FILES packaging/nootka-updater.1.gz DESTINATION share/man/man1)
    
endif(UNIX AND NOT APPLE)

###########################################################################
# uninstall target
###########################################################################
configure_file(
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
     "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

  add_custom_target(uninstall
   COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)


# add_subdirectory(mime) # target for make install-mime
add_subdirectory(packaging) # cpack rules


#=============================================================================
#  Mscore
#  Linux Music Score Editor
#  $Id:$
#
#  Copyright (C) 2002-2009 by Werner Schweer and others
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License version 2.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#=============================================================================

project(mscore)
CMAKE_MINIMUM_REQUIRED(VERSION 2.6.0)

cmake_policy(SET CMP0003 NEW)

# for debugging the make system uncomment next line:
# set(CMAKE_VERBOSE_MAKEFILE ON)

set(MSCORE_UNSTABLE  FALSE)                # mark as unstable

set(GCC_VISIBILITY TRUE)                  # requires gcc 4.x?
set(HAS_AUDIOFILE TRUE)                   # requires libsndfile

set(BUILD_SCRIPTGEN TRUE)                 #Generate Qt script bindings. (collides with qtscriptgenerator project on several distrib)
set(STATIC_SCRIPT_BINDINGS FALSE)
set(USE_SSE FALSE)

if (MINGW)
      set(GCC_VISIBILITY FALSE)           # we use gcc 3.6
      set(STATIC_SCRIPT_BINDINGS TRUE)
endif (MINGW)

if (APPLE)
      set(HAS_AUDIOFILE TRUE)            # requires libsndfile
      set(STATIC_SCRIPT_BINDINGS TRUE)
      set(CMAKE_OSX_ARCHITECTURES i386 ppc)   # build universal library
      set(CMAKE_OSX_SYSROOT /Developer/SDKs/MacOSX10.4u.sdk)
      set(CMAKE_OSX_DEPLOYMENT_TARGET 10.4) #min version required
      find_library(AudioToolboxFW NAMES AudioToolbox)
      find_library(AudioUnitFW NAMES AudioUnit)
      find_library(CoreAudioFW NAMES CoreAudio)
      find_library(CoreMidiFW NAMES CoreMIDI)
      find_library(SystemConfigurationFW NAMES SystemConfiguration)
      find_library(CoreServicesFW NAMES CoreServices)
      set(OsxFrameworks ${AudioToolboxFW} ${AudioUnitFW} ${CoreAudioFW} ${CoreMidiFW} ${SystemConfigurationFW} ${CoreServicesFW})
endif (APPLE)

set(CMAKE_INCLUDE_CURRENT_DIR TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH ON)
set(CMAKE_SKIP_RULE_DEPENDENCY TRUE)

# The Mscore version number.
SET(Mscore_VERSION_MAJOR  "1")
SET(Mscore_VERSION_MINOR  "1")
SET(Mscore_VERSION_PATCH  "0")

SET(Mscore_VERSION       "${Mscore_VERSION_MAJOR}.${Mscore_VERSION_MINOR}")
#SET(Mscore_VERSION_FULL  "${Mscore_VERSION}.${Mscore_VERSION_PATCH}")
SET(Mscore_VERSION_FULL  "${Mscore_VERSION}")

if (MINGW OR APPLE)
  if(MINGW)
      SET(Mscore_INSTALL_NAME  "")
      SET(Mscore_SHARE_NAME    "./")
  else(MINGW)
      SET(Mscore_INSTALL_NAME  "Contents/Resources/")
      SET(Mscore_SHARE_NAME    "mscore.app/")
  endif(MINGW)
else (MINGW OR APPLE)
      SET(Mscore_INSTALL_NAME  "mscore-${Mscore_VERSION}/")
      SET(Mscore_SHARE_NAME    "share/")
endif (MINGW OR APPLE)

include ( ${PROJECT_SOURCE_DIR}/cmake/UsePkgConfig1.cmake )
include ( ${PROJECT_SOURCE_DIR}/cmake/TargetDoc.cmake)

##
##  look for Qt4
##

set(QT_MIN_VERSION "4.6.0")
set(QT_USE_QTXML     TRUE)
set(QT_USE_QTSVG     TRUE)
set(QT_USE_QTNETWORK TRUE)
set(QT_USE_QTUITOOLS TRUE)
set(QT_USE_QTDESIGNER TRUE)
set(QT_USE_QTSCRIPT   TRUE)
set(QT_USE_QTSCRIPTTOOLS   TRUE)
set(QT_USE_QTWEBKIT   TRUE)

find_package(Qt4 "4.6")

if (NOT QT4_FOUND)
      message(FATAL_ERROR "Fatal error: QT (version >= 4.6.0) required."
         " Cmake tries to detect QT4 by searching for 'qmake' in your PATH."
         "If you have QT4 installed, make sure qmake is found in your PATH. "
         "If you compiled QT4 yourself make sure your new qmake ist found"
         " _first_ in your PATH."
         )
else (NOT QT4_FOUND)
      message("Qt lib found at ${QT_LIBRARY_DIR}")
endif (NOT QT4_FOUND)


# Set QT_QTSCRIPT_INCLUDE_DIR
FIND_PATH(QT_QTSCRIPT_INCLUDE_DIR QtScript
   PATHS
   ${QT_INCLUDE_DIR}/QtScript
   ${QT_LIBRARY_DIR}/QtScript.framework/Headers
   NO_DEFAULT_PATH
   )

# Set QT_QTSCRIPT_LIBRARY (for cmake 2.4)
#FIND_LIBRARY(QT_QTSCRIPT_LIBRARY_RELEASE NAMES QtScript QtScript4 PATHS ${QT_LIBRARY_DIR}        NO_DEFAULT_PATH)
#FIND_LIBRARY(QT_QTSCRIPT_LIBRARY_DEBUG   NAMES QtScript QtScript_debug QtScriptd4 PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
#_QT4_ADJUST_LIB_VARS(QTSCRIPT)

# Set QT_QTSCRIPT_TOOLS_LIBRARY (apparently still needed on Ubuntu 9.10 - cmake 2.6.4)
if (NOT MINGW AND NOT APPLE)
FIND_LIBRARY(QT_QTSCRIPT_TOOLS_LIBRARY_RELEASE NAMES QtScriptTools QtScriptTools4 PATHS ${QT_LIBRARY_DIR}        NO_DEFAULT_PATH)
FIND_LIBRARY(QT_QTSCRIPT_TOOLS_LIBRARY_DEBUG   NAMES QtScriptTools QtScriptTools_debug QtScriptToolsd4 PATHS ${QT_LIBRARY_DIR} NO_DEFAULT_PATH)
_QT4_ADJUST_LIB_VARS(QTSCRIPT_TOOLS)
endif (NOT MINGW AND NOT APPLE)

include(${QT_USE_FILE})

##
## alsa >= 1.0.0
##

if (APPLE OR MINGW)
	message("Disabling ALSA support due to OS X or MINGW build.")
      set (USE_ALSA 0)
      set (HAS_MIDI 1)
else (APPLE OR MINGW)
      PKGCONFIG (alsa 1.0.0 ALSA_INCDIR ALSA_LIBDIR ALSA_LIB ALSA_CPP )
      if (NOT ALSA_INCDIR)
            message(FATAL_ERROR "Fatal error: ALSA >= 1.0.0 required")
      else (NOT ALSA_INCDIR)
	      message("Alsa detected.")
            set (USE_ALSA 1)
            set (HAS_MIDI 1)
      endif (NOT ALSA_INCDIR)
endif (APPLE OR MINGW)


##
## find jack >= JACK_MIN_VERSION
##

SET(JACK_LONGNAME "jack (jack audio connection kit)")
SET(JACK_MIN_VERSION "0.98.0")

SET(BUILD_JACK AUTO CACHE STRING "Build with support for ${JACK_LONGNAME}. jack >= ${JACK_MIN_VERSION} will be needed. [ON/OFF/AUTO]")
IF(BUILD_JACK)
     IF(MINGW)
           set (USE_JACK 1)
           IF("$ENV{PROCESSOR_ARCHITEW6432}" STREQUAL "")
              IF($ENV{PROCESSOR_ARCHITECTURE} STREQUAL "x86")
                 # "pure" 32-bit environment
                 set (JACK_INCDIR "C:/Program Files/Jack v1.9.7/includes")
                 set (JACK_LIB "C:/Program Files/Jack v1.9.7/lib/libjack.a")
              ELSE($ENV{PROCESSOR_ARCHITECTURE} STREQUAL "x86")
                 # "pure" 64-bit environment
                 set (JACK_INCDIR "C:/Program Files (x86)/Jack v1.9.7/includes")
                 set (JACK_LIB "C:/Program Files (x86)/Jack v1.9.7/lib/libjack.a")
              ENDIF($ENV{PROCESSOR_ARCHITECTURE} STREQUAL "x86")
           ELSE("$ENV{PROCESSOR_ARCHITEW6432}" STREQUAL "")
              IF("$ENV{PROCESSOR_ARCHITECTURE}" STREQUAL "x86")
                 # 32-bit program running with an underlying 64-bit environment
                 set (JACK_INCDIR "C:/Program Files (x86)/Jack v1.9.7/includes")
                 set (JACK_LIB "C:/Program Files (x86)/Jack v1.9.7/lib/libjack.a")
              ELSE("$ENV{PROCESSOR_ARCHITECTURE}" STREQUAL "x86")
                 # theoretically impossible case...
              ENDIF("$ENV{PROCESSOR_ARCHITECTURE}" STREQUAL "x86")
           ENDIF("$ENV{PROCESSOR_ARCHITEW6432}" STREQUAL "")

     ELSE(MINGW)
           PKGCONFIG(jack ${JACK_MIN_VERSION} JACK_INCDIR JACK_LIBDIR JACK_LIB JACK_CPP)
           IF(JACK_INCDIR)
                 MESSAGE(STATUS "${JACK_LONGNAME} >= ${JACK_MIN_VERSION} found")
                 SET(USE_JACK 1)
           ELSE(JACK_INCDIR)
                 MESSAGE(STATUS "${JACK_LONGNAME} >= ${JACK_MIN_VERSION} not found")
                 IF(NOT BUILD_JACK STREQUAL AUTO)
                       MESSAGE(SEND_ERROR "Error: jack support requested but not found (BUILD_JACK=${BUILD_JACK})")
                 ENDIF()
           ENDIF(JACK_INCDIR)
     ENDIF(MINGW)
ELSE(BUILD_JACK)
     MESSAGE(STATUS "${JACK_LONGNAME} support disabled")
ENDIF(BUILD_JACK)


##
## portaudio
##

if (MINGW)
      set ( USE_PORTAUDIO 1 )
      set ( USE_PORTMIDI  1 )
else (MINGW)
   PKGCONFIG (portaudio-2.0 19 PORTAUDIO_INCDIR PORTAUDIO_LIBDIR PORTAUDIO_LIB PORTAUDIO_CPP )
   if (PORTAUDIO_INCDIR)
	message("portaudio detected ${PORTAUDIO_INCDIR} ${PORTAUDIO_LIBDIR} ${PORTAUDIO_LIB}")
      set ( USE_PORTAUDIO 1 )
   else (PORTAUDIO_INCDIR)
      message("optional package portaudio-2.0 Version 19 not found (package portaudio19-dev)\n")
      set ( USE_PORTAUDIO 0 )
   endif (PORTAUDIO_INCDIR)
   if (APPLE)
      set ( USE_PORTMIDI  1 )
   else (APPLE)
      set (USE_PORTMIDI 0)
   endif (APPLE)
endif (MINGW)

##
## produce config.h file
##
configure_file (
      ${PROJECT_SOURCE_DIR}/config.h.in
      ${PROJECT_BINARY_DIR}/config.h
      )
if (NOT MINGW AND NOT APPLE)
      install( FILES packaging/mscore.desktop DESTINATION share/applications)
endif (NOT MINGW AND NOT APPLE)

set (subdirs al awl bww2mxml osdabzip rtf2html mscore share manual demos
      singleapp fluid)

if (BUILD_SCRIPTGEN)
      set (subdirs ${subdirs} scriptgen)
endif (BUILD_SCRIPTGEN)

if (MINGW OR APPLE)
      set (subdirs ${subdirs} portmidi)
endif (MINGW OR APPLE)

subdirs(${subdirs})


include_directories(
   ${PROJECT_SOURCE_DIR}
   ${PROJECT_SOURCE_DIR}/mscore
   ${PROJECT_BINARY_DIR}
   ${PROJECT_BINARY_DIR}/mscore
   ${ALSA_INCDIR}
   ${JACK_INCDIR}
   ${PORTAUDIO_INCDIR}
)

##
## create package target
##

include (InstallRequiredSystemLibraries)

  SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MuseScore is a full featured WYSIWYG score editor")
  SET(CPACK_PACKAGE_VENDOR "Werner Schweer and Others")
  SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/COPYING")
  SET(CPACK_RESOURCE_FILE_LICENSE    "${PROJECT_SOURCE_DIR}/COPYING")

  SET(CPACK_PACKAGE_VERSION_MAJOR "${Mscore_VERSION_MAJOR}")
  SET(CPACK_PACKAGE_VERSION_MINOR "${Mscore_VERSION_MINOR}")
  SET(CPACK_PACKAGE_VERSION_PATCH "${Mscore_VERSION_PATCH}")
  SET(CPACK_PACKAGE_INSTALL_DIRECTORY "MuseScore ${Mscore_VERSION_MAJOR}.${Mscore_VERSION_MINOR}")

  SET(CPACK_NSIS_COMPRESSOR "/FINAL /SOLID lzma")

  IF(MINGW)
      SET(CPACK_PACKAGE_INSTALL_DIRECTORY "MuseScore")
      # There is a bug in NSI that does not handle full unix paths properly. Make
      # sure there is at least one set of four (4) backlasshes.
      SET(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/mscore/data\\\\installerhead.bmp")
      SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\mscore.exe,0")
      SET(CPACK_NSIS_DISPLAY_NAME "MuseScore ${Mscore_VERSION_FULL} MuseScore score typesetter")
      SET(CPACK_NSIS_HELP_LINK "http://www.musescore.org/")
      SET(CPACK_NSIS_URL_INFO_ABOUT "http://www.musescore.org/")
      SET(CPACK_NSIS_CONTACT "ws@wschweer.de")
      SET(CPACK_NSIS_MODIFY_PATH OFF)
      SET(CPACK_STRIP_FILES "mscore.exe")

      # File types association:
      SET(CPACK_NSIS_DEFINES "!include ${PROJECT_SOURCE_DIR}/packaging\\\\FileAssociation.nsh")

      SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
          Push \\\"ATENDATA\\\"
          Push \\\"$INSTDIR\\\\share\\\\aten\\\"
          Call WriteEnvStr
      ")

      SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
          \\\${registerExtension} \\\"MuseScore File\\\" \\\".mscx\\\" \\\"\\\$INSTDIR\\\\bin\\\\mscore.exe\\\"
          \\\${registerExtension} \\\"Compressed MuseScore File\\\" \\\".mscz\\\" \\\"\\\$INSTDIR\\\\bin\\\\mscore.exe\\\"
      ")
      SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
          \\\${unregisterExtension} \\\".mscx\\\" \\\"MuseScore File\\\"
          \\\${unregisterExtension} \\\".mscz\\\" \\\"Compressed MuseScore File\\\"
      ")
  ELSE(MINGW)
      SET(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/mscore/data/mscore.bmp")
      SET(CPACK_STRIP_FILES "mscore")
      SET(CPACK_SOURCE_STRIP_FILES "")
  ENDIF(MINGW)

  SET(CPACK_SOURCE_PACKAGE_FILE_NAME "mscore")
  SET(CPACK_PACKAGE_FILE_NAME     "${CPACK_SOURCE_PACKAGE_FILE_NAME}-${Mscore_VERSION_FULL}")
  SET(CPACK_PACKAGE_EXECUTABLES   "mscore" "MuseScore")

  set(CPACK_DEBIAN_PACKAGE_NAME         "mscore")
  set(CPACK_DEBIAN_PACKAGE_VERSION      "${Mscore_VERSION_FULL}-0ubuntu0.1")
  set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "i386")

  set(CPACK_DEBIAN_PACKAGE_DEPENDS    "libqt4-core (>= 4.4), libqt4-gui (>= 4.4)")

  set(CPACK_DEBIAN_PACKAGE_MAINTAINER   "tsmithe@ubuntu.com")
  set(CPACK_DEBIAN_PACKAGE_SECTION      "devel")
  set(CPACK_DEBIAN_PACKAGE_PRIORITY     "optional")
  set(CPACK_DEBIAN_PACKAGE_RECOMMENDS   "")
  set(CPACK_DEBIAN_PACKAGE_SUGGESTS     "")

  set(CPACK_PACKAGE_CONTACT       "ws@wschweer.de")

if (MINGW)
  set(CPACK_GENERATOR             "NSIS")
  set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/packaging" ${CMAKE_MODULE_PATH}) # Use custom version of NSIS.InstallOptions.ini
else (MINGW)
  if (NOT APPLE)
    set(CPACK_GENERATOR             "DEB;TBZ2")
    set(CPACK_DEB "on")
  endif (NOT APPLE)
endif (MINGW)

include (CPack)

add_custom_target(lupdate
   COMMAND ${PROJECT_SOURCE_DIR}/gen-qt-projectfile ${PROJECT_SOURCE_DIR} > mscore.pro
   COMMAND lupdate -noobsolete ${PROJECT_BINARY_DIR}/mscore.pro
   WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
   )

add_custom_target(lrelease
   COMMAND ${PROJECT_SOURCE_DIR}/gen-qt-projectfile ${PROJECT_SOURCE_DIR} > mscore.pro
   COMMAND lrelease ${PROJECT_SOURCE_DIR}/share/locale/*.ts
   WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
   )


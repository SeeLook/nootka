project(nootka)
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_BUILD_TYPE Release)
find_package(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )
add_definitions(${QT_DEFINITIONS})

set(CMAKE_COLOR_MAKEFILE ON)

include_directories(${QT_INCLUDES})

SET(Nootka_VERSION_MAJOR  "0")
SET(Nootka_VERSION_MINOR  "8")
SET(Nootka_VERSION_PATCH  "alpha")

SET(Nootka_VERSION       "${Nootka_VERSION_MAJOR}.${Nootka_VERSION_MINOR}")
SET(Nootka_VERSION_FULL  "${Nootka_VERSION}.${Nootka_VERSION_PATCH}")

###########################################################################
########   Looking for Portaudio
###########################################################################
IF(PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)
  set(PORTAUDIO_FOUND TRUE)
ELSE(PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)
  find_path(PORTAUDIO_INCLUDE_DIR
    NAMES
      portaudio.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
  )
  IF(WIN32)
            find_library(PORTAUDIO_LIBRARY
	    NAMES
	      libportaudio-2.dll
	    PATHS
	      ${CMAKE_CURRENT_SOURCE_DIR}
	      ${CMAKE_CURRENT_BINARY_DIR}
	  )
  ELSE(WIN32)
	  find_library(PORTAUDIO_LIBRARY
	    NAMES
	      portaudio
	    PATHS
	      /usr/lib
	      /usr/local/lib
	      /opt/local/lib
	      /sw/lib
	      ${CMAKE_CURRENT_BINARY_DIR}
	      ${CMAKE_CURRENT_SOURCE_DIR}
  )
  ENDIF(WIN32)

  set(PORTAUDIO_INCLUDE_DIRS
    ${PORTAUDIO_INCLUDE_DIR}
  )
  set(PORTAUDIO_LIBRARIES
    ${PORTAUDIO_LIBRARY}
)

  IF(PORTAUDIO_INCLUDE_DIRS AND PORTAUDIO_LIBRARIES)
     set(PORTAUDIO_FOUND TRUE)
  ENDIF(PORTAUDIO_INCLUDE_DIRS AND PORTAUDIO_LIBRARIES)

  IF(PORTAUDIO_FOUND)
      message(STATUS "Portaudio was Found in:\n${PORTAUDIO_LIBRARIES}\n${PORTAUDIO_INCLUDE_DIRS}")
  ELSE(PORTAUDIO_FOUND)
      message(FATAL_ERROR "Could not find Portaudio\nTry call cmake with args:\n-DPORTAUDIO_LIBRARIES=/path_to/libportaudio.la(dll) -DPORTAUDIO_INCLUDE_DIRS=/path_to_dir_with/portaudio.h\nor put those files in build dir or source dir")
  ENDIF(PORTAUDIO_FOUND)
  mark_as_advanced(PORTAUDIO_INCLUDE_DIRS PORTAUDIO_LIBRARIES)
ENDIF(PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)

include_directories(${PORTAUDIO_INCLUDE_DIRS})

###########################################################################
#libraries and compiller definitions for RtMidi
###########################################################################
IF(UNIX)
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
ENDIF(UNIX)

IF(WIN32)
  add_definitions(-D__WINDOWS_MM__)
ENDIF(WIN32)

add_subdirectory(src)


###########################################################################
#INSTALLATION
###########################################################################
IF(WIN32)
    set(INS_PREF "")
ELSE(WIN32)
    set(INS_PREF "share/nootka")
ENDIF(WIN32)

INSTALL(FILES gpl DESTINATION "${INS_PREF}")
INSTALL(DIRECTORY picts DESTINATION "${INS_PREF}")
INSTALL(DIRECTORY fonts DESTINATION "${INS_PREF}")
FILE(GLOB langs "${CMAKE_CURRENT_SOURCE_DIR}/lang/*.qm")
INSTALL(FILES ${langs} DESTINATION "${INS_PREF}/lang")
FILE(GLOB snds "${CMAKE_CURRENT_SOURCE_DIR}/sounds/classical-guitar.wav")
INSTALL(FILES ${snds} DESTINATION "${INS_PREF}/sounds")

# files related with Linux (mime, desktop entry, icons)
if(UNIX AND NOT APPLE)
    INSTALL(FILES mime/nootka.desktop DESTINATION share/applications)
    INSTALL(FILES picts/nootka.svg picts/levelCreator.png picts/nootka-exam.png DESTINATION share/pixmaps)
    INSTALL( CODE"
      execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/mime )
      execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/mime/install-mime.sh ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}/mime ${CMAKE_CURRENT_SOURCE_DIR}/mime)" )
    INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/mime/nootka.xml DESTINATION share/mime/packages)
	INSTALL( CODE"
      execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/doc )
      execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/packaging/make-chlog.sh ${Nootka_VERSION_FULL} ${CMAKE_CURRENT_BINARY_DIR}/doc/changelog ${CMAKE_CURRENT_SOURCE_DIR})" )
	INSTALL(FILES "${CMAKE_CURRENT_BINARY_DIR}/doc/changelog.gz" DESTINATION share/doc/nootka)
	INSTALL(FILES copyright DESTINATION share/doc/nootka)
	INSTALL(FILES packaging/nootka.1.gz DESTINATION share/man/man1)
    
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


add_subdirectory(mime) # target for make install-mime
add_subdirectory(packaging) # cpack rules

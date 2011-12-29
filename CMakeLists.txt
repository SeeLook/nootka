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
SET(Nootka_VERSION_PATCH  "beta")

SET(Nootka_VERSION       "${Nootka_VERSION_MAJOR}.${Nootka_VERSION_MINOR}")
SET(Nootka_VERSION_FULL  "${Nootka_VERSION}.${Nootka_VERSION_PATCH}")

###########################################################################
########   Looking for QtMultimedia
###########################################################################
  IF(WIN32)
    include_directories(${QT_INCLUDES}/QtMultimedia)
  ELSE(WIN32)
    include_directories(/usr/include/QtMultimediaKit)
    include_directories(/usr/include/QtMobility)
  ENDIF(WIN32)
# include_directories(/usr/include/QtMobility)


find_library(QTMULTIMEDIAKIT_LIBRARY
                  NAMES
                        QtMultimediaKit
                  PATHS
                        /usr/lib
                        /usr/local/lib
                        /opt/local/lib
  )

mark_as_advanced(QTMULTIMEDIAKIT_LIBRARY)
message(STATUS "Qt Multimedia: ${QTMULTIMEDIAKIT_LIBRARY}")

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
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
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
      message(STATUS "Fftw3 was Found in:\n${FFTW3_LIBRARIES}\n${FFTW3_INCLUDE_DIRS}")
  ELSE(FFTW3_FOUND)
      message(FATAL_ERROR "Could not find Fftw3\nTry call cmake with args:\n-DFFTW3_LIBRARIES=/path_to/libfftw3.la(dll) -DFFTW3_INCLUDE_DIRS=/path_to_dir_with/fftw3.h\nor put those files in build dir or source dir")
  ENDIF(FFTW3_FOUND)
  mark_as_advanced(FFTW3_INCLUDE_DIRS FFTW3_LIBRARIES)
ENDIF(FFTW3_LIBRARIES AND FFTW3_INCLUDE_DIRS)

include_directories(${FFTW3_INCLUDE_DIRS})

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
    set(INS_PREF ".")
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

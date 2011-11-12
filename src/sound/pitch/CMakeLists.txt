project(pitch)
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_BUILD_TYPE Release)

#set(QT_USE_QTMULTIMEDIA 1)

find_package(Qt4 REQUIRED)

INCLUDE( ${QT_USE_FILE} )
add_definitions(${QT_DEFINITIONS})

include_directories(${QT_INCLUDES})
include_directories(/usr/include/QtMobility)
include_directories(/usr/include/QtMultimediaKit)
# include_directories( tartini )


find_library(QTMULTIMEDIAKIT_LIBRARY
                  NAMES
                        QtMultimediaKit
                  PATHS
                        /usr/lib
                        /usr/local/lib
                        /opt/local/lib
  )

mark_as_advanced(QTMULTIMEDIAKIT_LIBRARY)
message(STATUS "${QTMULTIMEDIAKIT_LIBRARY}")

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
	      fftw3.dll
	    PATHS
	      ${CMAKE_CURRENT_SOURCE_DIR}
	      ${CMAKE_CURRENT_BINARY_DIR}
	  )
  ELSE(WIN32)
	  find_library(FFTW3_LIBRARY
	    NAMES
	      fftw3
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
      message(FATAL_ERROR "Could not find Portaudio\nTry call cmake with args:\n-DFFTW3_LIBRARIES=/path_to/libfftw3.la(dll) -DFFTW3_INCLUDE_DIRS=/path_to_dir_with/fftw3.h\nor put those files in build dir or source dir")
  ENDIF(FFTW3_FOUND)
  mark_as_advanced(FFTW3_INCLUDE_DIRS FFTW3_LIBRARIES)
ENDIF(FFTW3_LIBRARIES AND FFTW3_INCLUDE_DIRS)

include_directories(${FFTW3_INCLUDE_DIRS})
###################################################################################################################

set( PITCH_SRCS 
  main.cpp
  mainwindow.cpp
  taudioin.cpp
  tpitchfinder.cpp
 # tartini
  tartini/channel.cpp
  tartini/analysisdata.cpp
  tartini/mytransforms.cpp
  tartini/notedata.cpp
  tartini/zoomlookup.cpp
  tartini/musicnotes.cpp
  tartini/bspline.cpp
  tartini/prony.cpp
  tartini/conversions.cpp
  tartini/useful.cpp
  tartini/fast_smooth.cpp
  tartini/equalloudness.cpp

  tartini/array1d.h
  tartini/array2d.h
  tartini/large_vector.h
  tartini/myassert.h
  tartini/SmartPtr.h
 # filters
  tartini/filters/Filter.h
  tartini/filters/FastSmoothedAveragingFilter.cpp
  tartini/filters/FixedAveragingFilter.cpp
  tartini/filters/GrowingAveragingFilter.cpp
  tartini/filters/IIR_Filter.cpp
)

set( PITCH_MOC_SRCS
  mainwindow.h
  taudioin.h
  tpitchfinder.h
)

QT4_WRAP_CPP( PITCH_MOCS ${PITCH_MOC_SRCS} )

#ADD_DEFINITIONS(-DQT_NO_DEBUG)

ADD_EXECUTABLE( pitch
        ${PITCH_SRCS}
        ${PITCH_MOCS}
  )

TARGET_LINK_LIBRARIES( pitch
        ${QT_QTCORE_LIBRARY}
        ${QT_QTGUI_LIBRARY}
        ${QTMULTIMEDIAKIT_LIBRARY}
        ${FFTW3_LIBRARIES}
        
)

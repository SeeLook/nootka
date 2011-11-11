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

  tartini/array1d.h
  tartini/array2d.h
  tartini/large_vector.h
  tartini/myassert.h
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
        
)

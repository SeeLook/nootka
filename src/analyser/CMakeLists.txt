project(nooanalyser)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_BUILD_TYPE release)
find_package(Qt4 REQUIRED)
include( ${QT_USE_FILE} )
add_definitions(${QT_DEFINITIONS})

include_directories(${QT_INCLUDES})



include_directories ( . ../src )

set( NOOANALYSER_SRCS
  main.cpp
  tanalyswindow.cpp
  tanalyswdg.cpp
)

set( NOOANALYSER_MOC_SRCS
	tanalyswindow.h
  tanalyswdg.h
)

qt4_wrap_cpp ( NOOANALYSER_MOCS ${NOOANALYSER_MOC_SRCS} )

add_definitions(-DQT_NO_DEBUG)

add_executable( nooanalyser
        ${NOOANALYSER_SRCS}
        ${NOOANALYSER_MOCS}
    
  )
target_link_libraries(nooanalyser
        ${QT_QTCORE_LIBRARY}
        ${QT_QTGUI_LIBRARY}
        
  )
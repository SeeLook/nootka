project(nooanalyser)
cmake_minimum_required(VERSION 2.8)
set(CMAKE_BUILD_TYPE release)
find_package(Qt4 REQUIRED)
include( ${QT_USE_FILE} )
add_definitions(${QT_DEFINITIONS})

include_directories(${QT_INCLUDES})



#include_directories ( . ${CMAKE_COURENT_SOURCE_DIR}/../src ${CMAKE_COURENT_SOURCE_DIR}/../src/exam ${CMAKE_COURENT_SOURCE_DIR}/../src/level )
include_directories ( .
    .. # Tnote, TfingerPos, Ttune
    ../exam # Texam, TQAunit, TQAtype
    ../level # TexamLevel
    ../score # TkeySignature
 )

set( NOOANALYSER_SRCS
  main.cpp
  tanalyswindow.cpp
  tchart.cpp
  taxis.cpp
#  tanalyswdg.cpp

#    texam.cpp
#    ../exam/tqatype.cpp
#    ../exam/tqaunit.cpp
#    ../tnote.cpp
#    ../ttune.cpp
#    ../tfingerpos.h
#    ../score/tkeysignature.cpp
#    texamlevel.cpp
   #../level/tlevelselector.cpp
)

set( NOOANALYSER_MOC_SRCS
   tanalyswindow.h
   tchart.h
#  tanalyswdg.h
    #../level/tlevelselector.h

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

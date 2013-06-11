project(scor)
cmake_minimum_required(VERSION 2.8)
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE release)
endif (NOT CMAKE_BUILD_TYPE)
find_package(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )
add_definitions(${QT_DEFINITIONS})

include_directories(${QT_INCLUDES})

if (CMAKE_BUILD_TYPE MATCHES 'release')
# set (CMAKE_CXX_FLAGS_RELEASE "-DQT_NO_DEBUG")
  add_definitions(-DQT_NO_DEBUG)
endif (CMAKE_BUILD_TYPE MATCHES 'release')


INCLUDE_DIRECTORIES (. score)

SET( SCORE_SRCS
  main.cpp
  mainwindow.cpp
  tclef.cpp
  tnote.cpp
  ttune.cpp
  
  tdropshadoweffect.cpp
  
  score/tpushbutton.cpp
  score/tsimplescore.cpp
  score/tmainscore.cpp
  score/tscorescene.cpp
  score/tscoreitem.cpp
  score/tscorestaff.cpp
  score/tscorepianostaff.cpp
  score/tscoreclef.cpp
  score/tscorenote.cpp
  score/tscorekeysignature.cpp
  score/tscorecontrol.cpp
  score/tclefselector.cpp
  score/tclefpreview.cpp
  score/tscorescordature.cpp
  score/tscoreview.cpp
  
  score/tkeysignature.cpp
)

SET( SCORE_MOC_SRCS
  mainwindow.h
  
  score/tpushbutton.h
  score/tsimplescore.h
  score/tmainscore.h
  score/tscorescene.h
  score/tscoreitem.h
  score/tscorestaff.h
  score/tscorepianostaff.h
  score/tscoreclef.h
  score/tscorenote.h
  score/tscorekeysignature.h
  score/tscorecontrol.h
  score/tclefselector.h
  score/tclefpreview.h
)


QT4_WRAP_CPP( SCORE_MOCS ${SCORE_MOC_SRCS} )

if(UNIX AND NOT APPLE) #Linux
    ADD_EXECUTABLE( scor
        ${SCORE_SRCS}
        ${SCORE_MOCS}
    )
  TARGET_LINK_LIBRARIES( scor
        ${QT_QTCORE_LIBRARY}
        ${QT_QTGUI_LIBRARY}
  )
endif(UNIX AND NOT APPLE)

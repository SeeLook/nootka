project(scor)
cmake_minimum_required(VERSION 2.8)
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE release)
endif (NOT CMAKE_BUILD_TYPE)
find_package(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )
add_definitions(${QT_DEFINITIONS})


include_directories(${QT_INCLUDES})

INCLUDE_DIRECTORIES (. score)

SET( SCORE_SRCS
  main.cpp
  mainwindow.cpp
  score/tsimplescore.cpp
  score/tscorestaff.cpp
  score/tscorescene.cpp
  score/tscoreitem.cpp
)

SET( SCORE_MOC_SRCS
  mainwindow.h
  score/tsimplescore.h
  score/tscorescene.h
  score/tscoreitem.h
  score/tscorestaff.h
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

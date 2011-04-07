project(nootka)
cmake_minimum_required(VERSION 2.8)
find_package(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )
add_definitions(${QT_DEFINITIONS})

#include_directories(${QT_INCLUDES})

SET( NOOTKA_SRCS
  ./src/main.cpp
  ./src/mainwindow.cpp
  ./src/settingsdialog.cpp
  ./src/tglobals.cpp
  ./src/tkeysignatureview.cpp
  ./src/tnote.cpp
  ./src/tnoteview.cpp
  ./src/tscorewidget.cpp
  ./src/tscorewidgetsimple.cpp
  ./src/tnotename.cpp
  ./src/tfingerboard.cpp
  ./src/ttune.cpp
  ./src/tfingerpos.h
  )
#SET( NOOTKA_HEADERS
#    ./src/tfingerpos.h
#)
  
SET( NOOTKA_MOC_SRCS
  ./src/mainwindow.h
  ./src/settingsdialog.h
  ./src/tkeysignatureview.h
  ./src/tnoteview.h
  ./src/tscorewidget.h
  ./src/tscorewidgetsimple.h
  ./src/tnotename.h
  ./src/tfingerboard.h
  )

SET( NOOTKA_RCCS
     nootka.qrc
)
  
  # enable warnings
#ADD_DEFINITIONS( -Wall )
#SET( QT_USE_QTSVG TRUE )
#
QT4_WRAP_CPP( NOOTKA_MOCS ${NOOTKA_MOC_SRCS} )

QT4_ADD_RESOURCES( NOOTKA_RCC_SRCS ${NOOTKA_RCCS} )
  

#   QT4_WRAP_UI( SAMPLE_UI_HDRS ${SAMPLE_UIS} )

ADD_DEFINITIONS(-DQT_NO_DEBUG)


#ADD_EXECUTABLE( nootka ${NOOTKA_SRCS} ${NOOTKA_MOC_SRCS} ${NOOTKA_RC_SRCS}}
ADD_EXECUTABLE( nootka
        ${NOOTKA_SRCS}
        ${NOOTKA_MOCS}
        ${NOOTKA_RCC_SRCS} )
TARGET_LINK_LIBRARIES(nootka
        ${QT_QTCORE_LIBRARY}
        ${QT_QTGUI_LIBRARY}
 #       ${QT_QTSVG_LIBRARY}
)
  

#INSTALLATION
INSTALL(TARGETS nootka DESTINATION bin)
INSTALL(DIRECTORY picts DESTINATION share/nootka)
INSTALL(DIRECTORY fonts DESTINATION share/nootka)

# uninstall target
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
    IMMEDIATE @ONLY)

add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)

# add_subdirectory(src)
# add_subdirectory(picts)

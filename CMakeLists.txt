project(nootka)

cmake_minimum_required(VERSION 2.8)

find_package(Qt4 REQUIRED)

add_definitions(${QT_DEFINITIONS})

include_directories(${QT_INCLUDES})

SET( NOOTKA_SRCS
  ./src/main.cpp
  ./src/mainwindow.cpp
  ./src/scoresetttingsdlg.cpp
  ./src/tglobals.cpp
  ./src/tkeysignatureview.cpp
  ./src/tnote.cpp
  ./src/tnoteview.cpp
  ./src/tscorewidget.cpp
  ./src/tscorewidgetsimple.cpp
  )
  
  # another list, this time it includes all header files that should be treated with moc
SET( NOOTKA_MOC_HDRS
  ./src/mainwindow.h
  ./src/scoresetttingsdlg.h
  ./src/tglobals.h
  ./src/tkeysignatureview.h
  ./src/tnote.h
  ./src/tnoteview.h
  ./src/tscorewidget.h
  ./src/tscorewidgetsimple.h
       
  )
  
  # some .ui files
#   SET( NOOTKA_UIS
#   )
#   
#  and finally an resource file
#SET( NOOTKA_RCS
 #      nootka.qrc
  #)
  
  # enable warnings
#ADD_DEFINITIONS( -Wall )
  
  # by default only QtCore and QtGui modules are enabled
  # other modules must be enabled like this:
#   SET( QT_USE_QT3SUPPORT TRUE )   
SET( QT_USE_QTSVG TRUE )
#   
  # this command finds Qt4 libraries and sets all required variables
  # note that it's Qt4, not QT4 or qt4
#   FIND_PACKAGE( Qt4 REQUIRED )
  
  # add some useful macros and variables
  # (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that contains a path to CMake script)
#INCLUDE( ${QT_USE_FILE} )
  
  # this command will generate rules that will run rcc on all files from SAMPLE_RCS
  # in result SAMPLE_RC_SRCS variable will contain paths to files produced by rcc
QT4_ADD_RESOURCES( NOOTKA_RC_SRCS ${NOOTKA_RCS} )
  
  # this will run uic on .ui files:
#   QT4_WRAP_UI( SAMPLE_UI_HDRS ${SAMPLE_UIS} )
  
  # and finally this will run moc:
QT4_WRAP_CPP( NOOTKA_MOC_SRCS ${NOOTKA_MOC_HDRS} )
  
  # we need this to be able to include headers produced by uic in our code
  # (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
#INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
  
  # here we instruct CMake to build "sample" executable from all of the source files

#ADD_EXECUTABLE( nootka ${NOOTKA_SRCS} ${NOOTKA_MOC_SRCS} ${NOOTKA_RC_SRCS}}
ADD_EXECUTABLE( nootka ${NOOTKA_SRCS} ${NOOTKA_MOC_SRCS} )
TARGET_LINK_LIBRARIES(nootka ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSVG_LIBRARY}) )
  



# add_subdirectory(src)
# add_subdirectory(picts)

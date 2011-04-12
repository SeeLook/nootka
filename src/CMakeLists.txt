
SET( NOOTKA_SRCS
  main.cpp
  mainwindow.cpp
  tsettingsdialogbase.cpp
  tglobals.cpp
  tkeysignatureview.cpp
  tnote.cpp
  tnoteview.cpp
  tscorewidget.cpp
  tscorewidgetsimple.cpp
  tnotename.cpp
  tfingerboard.cpp
  ttune.cpp
  tfingerpos.h
###### Settings
  settings/settingsdialog.cpp
###### Exam
  exam/examsettingsdlg.cpp
  exam/texamlevel.cpp
  exam/levelsettings.cpp
  exam/questionssettings.cpp
  exam/tlevelsummarywdg.cpp
  exam/rangesettings.cpp
  )
#SET( NOOTKA_HEADERS
#    tfingerpos.h
#)
  
SET( NOOTKA_MOC_SRCS
  mainwindow.h
  tsettingsdialogbase.h
  tkeysignatureview.h
  tnoteview.h
  tscorewidget.h
  tscorewidgetsimple.h
  tnotename.h
  tfingerboard.h
###### Settings
  settings/settingsdialog.h
###### Exam
  exam/examsettingsdlg.h
  exam/levelsettings.h
  exam/questionssettings.h
  exam/tlevelsummarywdg.h
  exam/rangesettings.h
  )

QT4_WRAP_CPP( NOOTKA_MOCS ${NOOTKA_MOC_SRCS} )

ADD_DEFINITIONS(-DQT_NO_DEBUG)

ADD_EXECUTABLE( nootka
        ${NOOTKA_SRCS}
        ${NOOTKA_MOCS}
#        ${NOOTKA_RCC_SRCS}
)
TARGET_LINK_LIBRARIES(nootka
        ${QT_QTCORE_LIBRARY}
        ${QT_QTGUI_LIBRARY}
 #       ${QT_QTSVG_LIBRARY}
)

IF (UNIX AND NOT APPLE)
  INSTALL(TARGETS nootka DESTINATION bin)

ENDIF (UNIX AND NOT APPLE)

############################################

INCLUDE_DIRECTORIES (. exam score settings )

SET( NOOTKA_SRCS
  main.cpp
  mainwindow.cpp
  tglobals.cpp
  tnote.cpp
  tnotename.cpp
  tfingerboard.cpp
  ttune.cpp
  tfingerpos.h
###### Score
  score/tnoteview.cpp
  score/tscorewidget.cpp
  score/tscorewidgetsimple.cpp
  score/tkeysignatureview.cpp
###### Settings
  settings/settingsdialog.cpp
  settings/tsettingsdialogbase.cpp
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
  tnotename.h
  tfingerboard.h
###### Score
  score/tnoteview.h
  score/tscorewidget.h
  score/tscorewidgetsimple.h
  score/tkeysignatureview.h
  score/tkeysignatureview.h
###### Settings
  settings/settingsdialog.h
  settings/tsettingsdialogbase.h
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

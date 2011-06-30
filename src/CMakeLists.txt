################################################

INCLUDE_DIRECTORIES (. exam info level score settings )

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
  score/tkeysignature.cpp
  score/tclefview.cpp
###### Settings
  settings/settingsdialog.cpp
  settings/tsettingsdialogbase.cpp
  settings/namesettings.cpp
  settings/tnotationradiogroup.cpp
  settings/scoresettings.cpp
###### Level
  level/examsettingsdlg.cpp
  level/texamlevel.cpp
  level/levelsettings.cpp
  level/questionssettings.cpp
  level/tlevelselector.cpp
  level/rangesettings.cpp
  level/tquestionaswdg.cpp
  level/tkeysigncombobox.cpp
  level/tlevelheaderwdg.cpp
###### Exam
  exam/tqatype.cpp
  exam/texamexecutor.cpp
  exam/tstartexamdlg.cpp
  exam/tqaunit.cpp
  exam/texamview.cpp
###### Info
  info/taboutnootka.cpp
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
  score/tclefview.h
###### Settings
  settings/settingsdialog.h
  settings/tsettingsdialogbase.h
  settings/namesettings.h
  settings/tnotationradiogroup.h
  settings/scoresettings.h
###### Level
  level/examsettingsdlg.h
  level/levelsettings.h
  level/questionssettings.h
  level/tlevelselector.h
  level/rangesettings.h
  level/tquestionaswdg.h
  level/tkeysigncombobox.h
  level/tlevelheaderwdg.h
###### Exam
  exam/tstartexamdlg.h
  exam/texamview.h
  exam/texamexecutor.h
###### Info
  info/taboutnootka.h
  )

QT4_WRAP_CPP( NOOTKA_MOCS ${NOOTKA_MOC_SRCS} )

ADD_DEFINITIONS(-DQT_NO_DEBUG)

IF(WIN32)
   SET(GUI_TYPE WIN32)
ENDIF(WIN32)


ADD_EXECUTABLE( nootka
	${GUI_TYPE}
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

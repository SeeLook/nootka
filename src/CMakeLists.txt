################################################

INCLUDE_DIRECTORIES (. exam info level score settings sound charts)

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
  score/tpushbutton.cpp
###### Settings
  settings/settingsdialog.cpp
  settings/tsettingsdialogbase.cpp
  settings/namesettings.cpp
  settings/tnotationradiogroup.cpp
  settings/scoresettings.cpp
  settings/examsettings.cpp
  settings/tcolorbutton.cpp
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
  exam/texam.cpp
  exam/texamsummary.cpp
  exam/texamparams.h
  exam/texecutorsupply.cpp
###### Info
  info/taboutnootka.cpp
  info/tfirstrunwizzard.cpp
  info/select7note.cpp
  info/texamhelp.cpp
  info/texpertanswerhelp.cpp
###### Charts
  charts/tanalysdialog.cpp
  charts/tchart.cpp
  charts/tabstractaxis.cpp
  charts/txaxis.cpp
  charts/tyaxis.cpp
  charts/tmainline.cpp
  charts/tquestionpoint.cpp
  charts/tmainchart.cpp
  charts/tstafflinechart.cpp
  charts/ttipchart.cpp
  charts/tnotepixmap.cpp
###### Sound
  sound/tsound.cpp
  sound/taudioout.cpp
  sound/taudioin.cpp
  sound/audiooutsettings.cpp
  sound/audioinsettings.cpp
  sound/RtMidi.cpp
  sound/taudioparams.h
  sound/tpitchfinder.cpp
  sound/tvolumemeter.cpp
  sound/tpitchview.cpp
  sound/tartiniparams.h
# tartini
  sound/tartini/channel.cpp
  sound/tartini/analysisdata.cpp
  sound/tartini/mytransforms.cpp
  sound/tartini/notedata.cpp
  sound/tartini/bspline.cpp
  sound/tartini/conversions.cpp
  sound/tartini/useful.cpp
  sound/tartini/fast_smooth.cpp
  sound/tartini/array1d.h
  sound/tartini/large_vector.h
  sound/tartini/SmartPtr.h
 # filters
  sound/tartini/filters/Filter.h
  sound/tartini/filters/FastSmoothedAveragingFilter.cpp
  sound/tartini/filters/FixedAveragingFilter.cpp
  sound/tartini/filters/GrowingAveragingFilter.cpp
  sound/tartini/filters/IIR_Filter.cpp
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
#   score/tkeysignatureview.h
  score/tclefview.h
  score/tpushbutton.h
###### Settings
  settings/settingsdialog.h
  settings/tsettingsdialogbase.h
  settings/namesettings.h
  settings/tnotationradiogroup.h
  settings/scoresettings.h
  settings/examsettings.h
  settings/tcolorbutton.h
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
  exam/texamsummary.h
  exam/texecutorsupply.h
###### Info
  info/taboutnootka.h
  info/tfirstrunwizzard.h
  info/select7note.h
  info/texamhelp.h
  info/texpertanswerhelp.h
###### Charts
  charts/tanalysdialog.h
  charts/tchart.h
  charts/tmainchart.h
  charts/tmainline.h
###### Sound
  sound/tsound.h
  sound/taudioout.h
  sound/taudioin.h
  sound/audiooutsettings.h
  sound/audioinsettings.h
  sound/tpitchfinder.h
  sound/tvolumemeter.h
  sound/tpitchview.h
  )


QT4_WRAP_CPP( NOOTKA_MOCS ${NOOTKA_MOC_SRCS} )

ADD_DEFINITIONS(-DQT_NO_DEBUG)


if(MINGW)
 # dress up win executable in nice nootka icon ONLY FOR MINGW !!!!!!!!!!!!!
    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/nootico.o
                       COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/../picts/nootka-icon.rc
                            -o ${CMAKE_CURRENT_BINARY_DIR}/nootico.o )
    SET(NOOTKA_SRCS ${NOOTKA_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/nootico.o)
    ADD_EXECUTABLE( nootka WIN32 ${NOOTKA_SRCS} ${NOOTKA_MOCS} nootico.o )
	TARGET_LINK_LIBRARIES(nootka
        ${QT_QTCORE_LIBRARY}
        ${QT_QTGUI_LIBRARY}
        ${QTMULTIMEDIA_LIBRARIES}
        ${FFTW3_LIBRARIES}
        winmm
	)
endif(MINGW)

if(UNIX AND NOT APPLE) #Linux
    ADD_EXECUTABLE( nootka
        ${NOOTKA_SRCS}
        ${NOOTKA_MOCS}
    #        ${NOOTKA_RCC_SRCS}
    )
	TARGET_LINK_LIBRARIES(nootka
        ${QT_QTCORE_LIBRARY}
        ${QT_QTGUI_LIBRARY}
        ${QTMULTIMEDIA_LIBRARIES}
        ${FFTW3_LIBRARIES}
#         ${QWT_LIBRARIES}
        ${ALSA_LIBRARY} ${PTHREAD_LIBRARY}
	)
endif(UNIX AND NOT APPLE)

if(APPLE)
    set (MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/packaging/mac/info.plist")
   set (ExeName nootka MACOSX_BUNDLE)
   set (MACOSX_BUNDLE_INFO_STRING Nootka release for Mac OSX)
   set (MACOSX_BUNDLE_ICON_FILE      "nootka.icns")
   set (MACOSX_BUNDLE_GUI_IDENTIFIER net.sourceforge.nootka)
   set (MACOSX_BUNDLE_LONG_VERSION_STRING ${Nootka_VERSION_FULL})
   set (MACOSX_BUNDLE_BUNDLE_NAME Nootka)
   set (MACOSX_BUNDLE_SHORT_VERSION_STRING ${Nootka_VERSION_FULL})
   set (MACOSX_BUNDLE_BUNDLE_VERSION ${Nootka_VERSION_FULL})
   set (MACOSX_BUNDLE_COPYRIGHT nootka.sf.net)
    ADD_EXECUTABLE( ${ExeName}
        ${NOOTKA_SRCS}
        ${NOOTKA_MOCS}
    )
  TARGET_LINK_LIBRARIES(nootka
        ${QT_QTCORE_LIBRARY}
        ${QT_QTGUI_LIBRARY}
        ${QTMULTIMEDIA_LIBRARIES}
        ${FFTW3_LIBRARIES}
#         ${QWT_LIBRARIES}
        ${CoreAudio_LIB}
        ${CoreMidi_LIB}
        ${CoreFoundation_LIB}        
  )
endif(APPLE)




if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
  install(TARGETS nootka DESTINATION bin)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS nootka DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS nootka BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX})
  endif(WIN32)
endif(UNIX AND NOT APPLE)


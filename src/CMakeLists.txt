################################################

add_subdirectory(updater)
add_subdirectory(sound/rt)

if(UNIX AND NOT APPLE)
  if(ALSA_LIBRARY)
    add_definitions(-D__LINUX_ALSA__)
    message(STATUS "support for ALSA\t\t TRUE")
  else(ALSA_LIBRARY)
    message(STATUS "support for ALSA\t\t FALSE")
  endif(ALSA_LIBRARY)
  if(ENABLE_JACK AND JACK_INCLUDE_DIR)
    add_definitions(-D__UNIX_JACK__)
    include_directories(${JACK_INCLUDE_DIR})
    message(STATUS "support for JACK\t\t TRUE")
  else(ENABLE_JACK AND JACK_INCLUDE_DIR)
    message(STATUS "support for JACK\t\t FALSE")
  endif(ENABLE_JACK AND JACK_INCLUDE_DIR)
  if(ENABLE_PULSEAUDIO AND PULSE_INCLUDE_DIR)
    add_definitions(-D__LINUX_PULSE__)
    include_directories(${PULSE_INCLUDE_DIR})
    message(STATUS "support for PulseAudio\t TRUE")
  else(ENABLE_PULSEAUDIO AND PULSE_INCLUDE_DIR)
    message(STATUS "support for PulseAudio\t FALSE")
  endif(ENABLE_PULSEAUDIO AND PULSE_INCLUDE_DIR)
endif(UNIX AND NOT APPLE)

INCLUDE_DIRECTORIES (. exam guitar info level music notename score settings sound charts )

SET( NOOTKA_SRCS
  main.cpp
  mainwindow.cpp
  tglobals.cpp  
###### music
	music/tnote.cpp
	music/tclef.cpp
	music/tkeysignature.cpp
	music/ttune.cpp
	music/tinstrument.cpp
###### guitar
	guitar/tfingerboard.cpp
	guitar/tfingerpos.cpp
###### notename
	notename/tnotename.cpp
	notename/tnamestylefilter.cpp
	notename/tnotenamelabel.cpp
###### widgets
#	widgets/tpixmapradio.cpp
	widgets/troundedlabel.cpp
	widgets/tcolorbutton.cpp
###### sound
  sound/tsound.cpp
  sound/tmidiout.cpp
  sound/audiooutsettings.cpp
  sound/audioinsettings.cpp
  sound/taudioparams.h
  sound/tpitchfinder.cpp
  sound/tabstractsoundview.cpp
  sound/tpitchview.cpp
  sound/tintonationview.cpp
  sound/tvolumeview.cpp
  sound/tartiniparams.h
  sound/tabstractplayer.cpp
  sound/toggscale.cpp
  sound/trtaudioabstract.cpp
  sound/trtaudioout.cpp
  sound/trtaudioin.cpp
  sound/rt/RtAudio.cpp
  sound/rt/RtMidi.cpp
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
###### Score
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
  score/tscorescordature.cpp
  score/tscoreview.cpp
###### Settings
  settings/tsettingsdialog.cpp
  settings/tsettingsdialogbase.cpp
  settings/tglobalsettings.cpp
  settings/tnotenamesettings.cpp
  settings/tnotationradiogroup.cpp
  settings/tscoresettings.cpp
  settings/texamsettings.cpp
  settings/tguitarsettings.cpp
  settings/tupdateprocess.cpp
  settings/tvolumeslider.cpp
  settings/tselectclef.cpp
  settings/tscalepreviewlabel.cpp
###### Level
  level/tlevelcreatordlg.cpp
  level/texamlevel.cpp
  level/levelsettings.cpp
  level/questionssettings.cpp
  level/accidsettings.cpp
  level/tlevelselector.cpp
  level/rangesettings.cpp
  level/tquestionaswdg.cpp
  level/tkeysigncombobox.cpp
  level/tlevelheaderwdg.cpp
  level/tlevelpreview.cpp  
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
  exam/tquestiontip.cpp
  exam/tcanvas.cpp
  exam/tprogresswidget.cpp
  exam/tgroupedqaunit.cpp
  exam/tnootkacertificate.cpp
  exam/tglobalexamstore.cpp
  exam/texercises.cpp
###### Info
  info/thelpdialogbase.cpp
  info/taboutnootka.cpp
  info/tfirstrunwizzard.cpp
  info/select7note.cpp
  info/texamhelp.cpp
  info/texpertanswerhelp.cpp
  info/tsupportnootka.cpp
  info/tnootkalabel.cpp
  info/tpixmaker.cpp
  info/tmainhelp.cpp
###### Charts
  charts/tanalysdialog.cpp
  charts/tchart.cpp
  charts/tabstractaxis.cpp
  charts/txaxis.cpp
  charts/tyaxis.cpp
  charts/tmainline.cpp
  charts/tquestionpoint.cpp
  charts/tmainchart.cpp
  charts/tlinearchart.cpp
  charts/tbarchart.cpp
  charts/tbar.cpp
  charts/tstafflinechart.cpp
  charts/ttipchart.cpp
  charts/tnotepixmap.cpp
  charts/tgraphicstexttip.cpp
  charts/tgraphicsline.cpp
  charts/sorting.cpp
  charts/tdropshadoweffect.cpp
  charts/tstatisticstip.cpp
  charts/ttiphandler.cpp
###### Animations
	animations/tstrikedoutitem.cpp
	animations/tanimedtextitem.cpp
	animations/tcrossfadetextanim.cpp
	animations/tanimeditem.cpp
	animations/tblinkingitem.cpp
  )
  
# if(MINGW)
#   LIST(APPEND NOOTKA_SRCS "sound/rt/asio.cpp"  "sound/rt/asiodrivers.cpp"  "sound/rt/asiolist.cpp"  "sound/rt/iasiothiscallresolver.cpp")
# endif(MINGW)

if(APPLE OR MINGW) # SoundTouch
  LIST(APPEND NOOTKA_SRCS "sound/stouch/SoundTouch.cpp" "sound/stouch/TDStretch.cpp" "sound/stouch/RateTransposer.cpp"
    "sound/stouch/AAFilter.cpp" "sound/stouch/FIRFilter.cpp" "sound/stouch/FIFOSampleBuffer.cpp"
    "sound/stouch/cpu_detect_x86.cpp" "sound/stouch/sse_optimized.cpp")
endif(APPLE OR MINGW)

SET( NOOTKA_MOC_SRCS
  mainwindow.h  
###### Sound
  sound/tsound.h
  sound/tmidiout.h
  sound/audiooutsettings.h
  sound/audioinsettings.h
  sound/tpitchfinder.h
  sound/toggscale.h
  sound/tintonationview.h
  sound/tpitchview.h
  sound/tabstractplayer.h
  sound/trtaudioout.h
  sound/trtaudioin.h
  sound/tvolumeview.h
###### Score
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
###### guitar
	guitar/tfingerboard.h	
###### notename
	notename/tnotename.h
	notename/tnotenamelabel.h
###### widgets
#	widgets/tpixmapradio.h
	widgets/tcolorbutton.h
###### Settings
  settings/tsettingsdialog.h
  settings/tsettingsdialogbase.h
  settings/tglobalsettings.h
  settings/tnotenamesettings.h
  settings/tnotationradiogroup.h
  settings/tguitarsettings.h
  settings/tscoresettings.h
  settings/texamsettings.h
  settings/tupdateprocess.h
  settings/tvolumeslider.h
  settings/tselectclef.h
###### Level
  level/tlevelcreatordlg.h
  level/levelsettings.h
  level/questionssettings.h
  level/accidsettings.h
  level/tlevelselector.h
  level/rangesettings.h
  level/tquestionaswdg.h
  level/tkeysigncombobox.h
  level/tlevelheaderwdg.h
  level/tlevelpreview.h
###### Exam
  exam/tstartexamdlg.h
  exam/texamview.h
  exam/texamexecutor.h
  exam/texamsummary.h
  exam/texecutorsupply.h
  exam/tcanvas.h
  exam/tprogresswidget.h
  exam/tquestiontip.h
  exam/tnootkacertificate.h
  exam/texercises.h
###### Info
  info/taboutnootka.h
  info/tfirstrunwizzard.h
  info/select7note.h
  info/texamhelp.h
  info/texpertanswerhelp.h
  info/tsupportnootka.h
  info/thelpdialogbase.h
###### Charts
  charts/tanalysdialog.h
  charts/tchart.h
  charts/tmainchart.h
  charts/ttiphandler.h
###### Animations
	animations/tstrikedoutitem.h
	animations/tanimedtextitem.h
	animations/tanimeditem.h
	animations/tcrossfadetextanim.h
	animations/tblinkingitem.h
)
  

QT4_WRAP_CPP( NOOTKA_MOCS ${NOOTKA_MOC_SRCS} )

if(UNIX AND NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-z,relro")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,relro")
endif(UNIX AND NOT APPLE)


if(MINGW)
  set(CMAKE_CXX_FLAGS "-O2 -march=pentium3m") # SoundTouch require it
 # dress up win executable in nice nootka icon ONLY FOR MINGW !!!!!!!!!!!!!
    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/nootico.o
                       COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/../picts/nootka-icon.rc
                            -o ${CMAKE_CURRENT_BINARY_DIR}/nootico.o )
    SET(NOOTKA_SRCS ${NOOTKA_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/nootico.o)
    ADD_EXECUTABLE( nootka WIN32 ${NOOTKA_SRCS} ${NOOTKA_MOCS} nootico.o )
	TARGET_LINK_LIBRARIES(nootka
        ${QT_QTCORE_LIBRARY}
        ${QT_QTGUI_LIBRARY}
        ${FFTW3_LIBRARIES}
        ${VORBIS_LIBRARIES}
#         ${SOUNDTOUCH_LIBRARY}
        winmm dsound
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
        ${FFTW3_LIBRARIES}
        ${ALSA_LIBRARY} ${PTHREAD_LIBRARY}
        ${JACK_LIBRARY}
        ${PULSESIMPLE_LIBRARY} ${PULSEERROR_LIBRARY}
        ${VORBIS_LIBRARIES}
        ${SOUNDTOUCH_LIBRARY}
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
        ${QT_QTNETWORK_LIBRARY} # for macdeployqt
        ${FFTW3_LIBRARIES}
        ${CoreAudio_LIB}
        ${CoreMidi_LIB}
        ${CoreFoundation_LIB}
        ${VORBIS_LIBRARIES}
#         ${SOUNDTOUCH_LIBRARY}
  )
endif(APPLE)




if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
  install(TARGETS nootka DESTINATION bin)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS nootka DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS nootka BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX})
    install( CODE "
    execute_process(COMMAND echo copying info.plist )
    execute_process(COMMAND sh ${CMAKE_SOURCE_DIR}/packaging/mac/grabInfo.sh ${Nootka_VERSION_FULL} ${CMAKE_SOURCE_DIR} ${CMAKE_INSTALL_PREFIX} )"
    )
  endif(WIN32)
endif(UNIX AND NOT APPLE)


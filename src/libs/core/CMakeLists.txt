########################################################################
# CMakeList.txt NootkaCore library
########################################################################

add_definitions(-DNOOTKACORE_LIBRARY)

set(LIB_NOOTKACORE_SRC
  tinitcorelib.cpp
  texamparams.h
  taudioparams.h
  tscoreparams.h
  tcolor.cpp
  tfingerpos.cpp
  tglobals.cpp
  tnoofont.cpp
  tpath.cpp
  tmtr.cpp

  animations/tabstractanim.cpp
  animations/tblinkingitem.cpp
  animations/tcoloredanim.cpp
  animations/tcombinedanim.cpp
  animations/tfadeanim.cpp
  animations/tmorphedanim.cpp
  animations/tmovedanim.cpp
  animations/tscaledanim.cpp
  animations/tcrossfadetextanim.cpp

  music/tnote.cpp
  music/trhythm.cpp
  music/tnamestylefilter.cpp
  music/ttune.cpp
  music/tclef.cpp
  music/tinstrument.cpp
  music/tkeysignature.cpp
  music/tchunk.cpp
  music/tmeasure.cpp
  music/tmelody.cpp
  music/tmeter.cpp

  graphics/tdropshadoweffect.cpp
  graphics/tgraphicstexttip.cpp

  exam/tqatype.cpp
  exam/tqaunit.cpp
  exam/tqagroup.cpp
  exam/tattempt.cpp
  exam/tlevel.cpp
  exam/texam.cpp
  exam/textrans.h
  exam/tresulttext.cpp

  widgets/tpushbutton.cpp

  touch/ttouchproxy.cpp
  touch/ttoucharea.cpp
  touch/ttouchparams.cpp

  plugins/tplugininterface.h
  plugins/tpluginsloader.cpp
)

add_library(NootkaCore SHARED ${LIB_NOOTKACORE_SRC} )
target_link_libraries(NootkaCore Qt5::Widgets)

if(UNIX AND NOT APPLE)   # Linux path for Nootka library
  install(TARGETS NootkaCore DESTINATION lib/nootka)
else(UNIX AND NOT APPLE)
  if(WIN32)       # Windows
    install(TARGETS NootkaCore DESTINATION .)
  else(WIN32)       # MacOs
    install(TARGETS NootkaCore DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks")
  endif(WIN32)
endif(UNIX AND NOT APPLE)



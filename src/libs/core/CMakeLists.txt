########################################################################
# CMakeList.txt NootkaCore library
########################################################################

add_definitions(-DNOOTKACORE_LIBRARY)


find_package(ZLIB REQUIRED)

set(LIB_NOOTKACORE_SRC
  tinitcorelib.cpp
  texamparams.h
  taudioparams.h
  tscoreparams.h
  tcolor.cpp
  tfingerpos.cpp
  tglobals.cpp
  tpath.cpp
  tmtr.cpp
  tnootkaqml.cpp
  taction.cpp
  tnoofont.cpp

  minizip/tzip.cpp
  minizip/zip.c
  minizip/unzip.c
  minizip/ioapi.c

  music/tnote.cpp
  music/trhythm.cpp
  music/tnamestylefilter.cpp
  music/ttune.cpp
  music/ttuneobject.cpp
  music/tclef.cpp
  music/tinstrument.cpp
  music/tkeysignature.cpp
  music/tchunk.cpp
  music/tmeasure.cpp
  music/tmelody.cpp
  music/tmeter.cpp
  music/tnotestruct.cpp
  music/ttechnical.cpp
  music/trtmgroup.cpp
  music/timportscore.cpp

  score/tscoreobject.cpp
  score/tstaffitem.cpp
  score/tmeasureobject.cpp
  score/tnoteitem.cpp
  score/tnotepair.cpp
  score/tbeamobject.cpp
  score/tstafflines.cpp
  score/taddnoteitem.cpp
  score/tmelodypreview.cpp
  score/tdummychord.cpp

  instruments/tcommoninstrument.cpp
  instruments/tguitarbg.cpp
  instruments/tpianobg.cpp
  instruments/tbandoneonbg.cpp
  instruments/tsaxbg.cpp

  exam/tqatype.cpp
  exam/tqaunit.cpp
  exam/tqagroup.cpp
  exam/tattempt.cpp
  exam/tlevel.cpp
  exam/texam.cpp
  exam/textrans.h
  exam/tresulttext.cpp

  Android/tfiledialog.cpp
)

if (WIN32)
  list(APPEND LIB_NOOTKACORE_SRC minizip/iowin32.c)
endif()

if (WITH_STATIC_LIB)
  add_library(NootkaCore STATIC ${LIB_NOOTKACORE_SRC} )
else ()
  add_library(NootkaCore SHARED ${LIB_NOOTKACORE_SRC} )
endif ()
target_link_libraries(NootkaCore
  Qt5::Widgets
  Qt5::Qml
  Qt5::Quick
  Qt5::QuickControls2
  ZLIB::ZLIB
)


if (NOT WITH_STATIC_LIB)
  if(UNIX AND NOT APPLE)   # Linux path for Nootka library
    install(TARGETS NootkaCore DESTINATION lib/nootka)
  else(UNIX AND NOT APPLE)
    if(WIN32)       # Windows
      install(TARGETS NootkaCore DESTINATION .)
    else(WIN32)       # MacOs
      install(TARGETS NootkaCore DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks")
    endif(WIN32)
  endif(UNIX AND NOT APPLE)
endif (NOT WITH_STATIC_LIB)



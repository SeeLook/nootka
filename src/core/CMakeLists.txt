########################################################################
# CMakeList.txt NootkaCore library
########################################################################

add_definitions(-DNOOTKACORE_LIBRARY)

if (WIN32)
  set(CMAKE_FIND_LIBRARY_SUFFIXES ".dll")

  if (NOT ZLIB_INCLUDE_DIR)
    find_path(ZLIB_INCLUDE_DIR
      NAMES
        "zlib.h"
      PATHS
        ${CMAKE_SOURCE_DIR}/zlib/
        ${CMAKE_SOURCE_DIR}/../zlib/
        ${CMAKE_BINARY_DIR}/zlib/
        ${CMAKE_BINARY_DIR}/../zlib/
        /usr/x86_64-w64-mingw32/include/
    )
    include_directories(${ZLIB_INCLUDE_DIR})
  endif()

  if (NOT ZLIB_LIBRARY)
    find_library(ZLIB_LIBRARY
      NAMES
        zlib zlib1
      PATHS
        ${CMAKE_SOURCE_DIR}/zlib/
        ${CMAKE_SOURCE_DIR}/../zlib/
        ${CMAKE_BINARY_DIR}/zlib/
        ${CMAKE_BINARY_DIR}/../zlib/
    )
  endif()
endif()

if (NOT ZLIB_LIBRARY OR NOT ZLIB_INCLUDE_DIR)
  find_package(ZLIB REQUIRED)
endif()

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
  # taction.cpp
  tnoofont.cpp

  minizip/tzip.cpp
  minizip/zip.c
  minizip/unzip.c
  minizip/ioapi.c

  # music/tnote.cpp
  # music/trhythm.cpp
  music/tnamestylefilter.cpp
  # music/ttune.cpp
  music/ttuneobject.cpp
  # music/tclef.cpp
  music/tinstrument.cpp
  music/tkeysignature.cpp
  music/tchunk.cpp
  music/tmeasure.cpp
  music/tmelody.cpp
  # music/tmeter.cpp
  music/tnotestruct.cpp
  music/ttechnical.cpp
  music/trtmgroup.cpp
  music/timportscore.cpp

  # score/tscoreobject.cpp
  # score/tstaffitem.cpp
  # score/tmeasureobject.cpp
  # score/tnoteitem.cpp
  # score/tnotepair.cpp
  # score/tbeamobject.cpp
  # score/tstafflines.cpp
  # score/taddnoteitem.cpp
  # score/tmelodypreview.cpp
  # score/tdummychord.cpp

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


target_sources(nootka PUBLIC
    ${LIB_NOOTKACORE_SRC}
)

target_link_libraries(nootka PUBLIC
    ${ZLIB_LIBRARY}
)



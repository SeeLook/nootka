########################################################################
# CMakeList.txt NootkaMain library
########################################################################


add_definitions(-DNOOTKAMAIN_LIBRARY)

include_directories( . ../core ../score ../widgets ../sound ../misc )

set(LIB_NOOTKAMAIN_SRC
  animations/tstrikedoutitem.cpp

  gui/tmainview.cpp
  gui/tmelman.cpp
  gui/ttoolbar.cpp
  gui/tmenu.cpp
  gui/tstatuslabel.cpp
  gui/tbgpixmap.cpp

  guitar/tfingerboard.cpp
  guitar/tguitarview.cpp

  notename/tnotename.cpp
  notename/tnotenamelabel.cpp
  notename/tnametip.cpp

  score/tmainscore.cpp
  score/tscoreactions.cpp

  exam/tprogresswidget.cpp
  exam/texamview.cpp
  exam/trandmelody.cpp
)


add_library(NootkaMain SHARED ${LIB_NOOTKAMAIN_SRC} )
target_link_libraries(NootkaMain Qt5::Widgets NootkaMisc NootkaSound )

if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
  install(TARGETS NootkaMain DESTINATION lib/nootka)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS NootkaMain DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS NootkaMain DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks")
  endif(WIN32)
endif(UNIX AND NOT APPLE)



########################################################################
# CMakeList.txt NootkaScore library
########################################################################

include_directories( . ../core )

add_definitions(-DNOOTKACORE_LIBRARY)

set(LIB_NOOTKASCORE_SRC
  graphics/tnotepixmap.cpp

  widgets/tselectclef.cpp

  tscoreitem.cpp
  tscoreclef.cpp
  tscorekeysignature.cpp
  tscorenote.cpp
  tnoteitem.cpp
  tscorescene.cpp
  tscorescordature.cpp
  tscorestaff.cpp
  tscoremeasure.cpp
  tscorebeam.cpp
  tscoretie.cpp
  tscore5lines.cpp
  tnotecontrol.cpp
  tpaneitem.cpp
  trhythmpane.cpp
  tscorelines.cpp
  tscoremeter.cpp
  tsimplescore.cpp
  tmultiscore.cpp
)

add_library(NootkaScore SHARED ${LIB_NOOTKASCORE_SRC} )
target_link_libraries(NootkaScore NootkaCore Qt5::Widgets)

if(UNIX AND NOT APPLE) 	# Linux path for Nootka library
  install(TARGETS NootkaScore DESTINATION lib/nootka)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS NootkaScore DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS NootkaScore DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks")
  endif(WIN32)
endif(UNIX AND NOT APPLE)



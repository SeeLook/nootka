########################################################################
# CMakeList.txt NootkaWidgets library
########################################################################

include_directories( . ../core ../score )

add_definitions(-DNOOTKACORE_LIBRARY)

set(LIB_NOOTKAWIDGETS_SRC
	level/tlevelsdefs.cpp
	level/tlevelpreview.cpp
	level/tlevelselector.cpp
	level/tfixleveldialog.cpp

	widgets/tquestionaswdg.cpp
	widgets/tsettingsdialogbase.cpp
	widgets/troundedlabel.cpp
	widgets/tselectinstrument.cpp
	widgets/thelpdialogbase.cpp
)

add_library(NootkaWidgets SHARED ${LIB_NOOTKAWIDGETS_SRC} )
target_link_libraries(NootkaWidgets NootkaScore Qt5::Widgets)

if(UNIX AND NOT APPLE) 	# Linux path for Nootka library
  install(TARGETS NootkaWidgets DESTINATION lib/nootka)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS NootkaWidgets DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS NootkaWidgets DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks")
  endif(WIN32)
endif(UNIX AND NOT APPLE)



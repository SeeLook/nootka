########################################################################
# CMakeList.txt NootkaMisc library
########################################################################


add_definitions(-DNOOTKAMISC_LIBRARY)

include_directories( . ../core ../score ../widgets )

set(LIB_NOOTKAMISC_SRC
	select7note.cpp
	tabout.cpp
	tnootkalabel.cpp
	tscalepreviewlabel.cpp
	tmisctrans.h

	help/thelpdialogbase.cpp
	help/texamhelp.cpp
	help/texpertanswerhelp.cpp
	help/tmainhelp.cpp
)


add_library(NootkaMisc SHARED ${LIB_NOOTKAMISC_SRC} )
target_link_libraries(NootkaMisc Qt5::Widgets NootkaScore NootkaWidgets )

if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
  install(TARGETS NootkaMisc DESTINATION lib/nootka)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS NootkaMisc DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS NootkaMisc DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks")
  endif(WIN32)
endif(UNIX AND NOT APPLE)



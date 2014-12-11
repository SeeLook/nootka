########################################################################
# CMakeList.txt NootkaMisc library
########################################################################


add_definitions(-DNOOTKAMISC_LIBRARY)

include_directories( . ../core )

set(LIB_NOOTKAMISC_SRC
	select7note.cpp
	taboutnootka.cpp
	tfirstrunwizzard.cpp
	tnootkalabel.cpp
	tscalepreviewlabel.cpp
	tsupportnootka.cpp
	
	help/thelpdialogbase.cpp
	help/texamhelp.cpp
	help/texpertanswerhelp.cpp
	help/tmainhelp.cpp
)

IF(WIN32)
	list(APPEND LIB_NOOTKAMISC_SRC
    main.cpp
	)
endif(WIN32)


add_library(NootkaMisc SHARED ${LIB_NOOTKAMISC_SRC} )
target_link_libraries(NootkaMisc Qt5::Widgets NootkaCore )

if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
  install(TARGETS NootkaMisc DESTINATION lib)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS NootkaMisc DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS NootkaMisc BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX})
  endif(WIN32)
endif(UNIX AND NOT APPLE)



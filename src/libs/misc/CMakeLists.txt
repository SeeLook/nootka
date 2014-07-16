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
# qt5_use_modules(NootkaSound Widgets)


# get_directory_property( CoreDIrDefs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMPILE_DEFINITIONS )
# message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ${CoreDIrDefs}" )




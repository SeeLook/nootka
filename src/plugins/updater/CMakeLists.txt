########################################################################
# CMakeList.txt nootka updater plugin
########################################################################

option(ENABLE_UPDATER "Enable online checking for new Nootka versions" ON)

if(ENABLE_UPDATER)
  message(STATUS "check for updates\t\t TRUE")
  find_package(Qt5Network)
    SET( UPDATER_SRCS
      tupdaterplugin.cpp
      tupdatechecker.cpp
      updatefunctions.cpp
      tupdateruleswdg.cpp
      tupdatesummary.cpp
		)

	add_library(NootkaUpdaterPlugin SHARED ${UPDATER_SRCS} )
	target_link_libraries(NootkaUpdaterPlugin Qt5::Widgets Qt5::Network )


	if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
		install(TARGETS NootkaUpdaterPlugin DESTINATION /lib/nootka)
	else(UNIX AND NOT APPLE)
		if(WIN32) 			# Windows
			install(TARGETS nootka-updater DESTINATION .)
		else(WIN32) 			# MacOs
			install(TARGETS nootka-updater DESTINATION "nootka.app/Contents/MacOS/")
		endif(WIN32)
	endif(UNIX AND NOT APPLE)
    
else(ENABLE_UPDATER)
    message(STATUS "check for updates\t\t FALSE")
endif(ENABLE_UPDATER)

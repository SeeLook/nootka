########################################################################
# CMakeList.txt src
########################################################################

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)


find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

# Force using c++11 if not set already - now it is mandatory
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(UNIX AND NOT APPLE)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro")
	set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-z,relro")
	set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,relro")
# rpath rules
  option(OBS_SUSE_QUIRKS "Must be ON for Suse machines in Open Build Services" OFF)
	set(NOOTKA_LIB_PATH "${CMAKE_INSTALL_PREFIX}/lib/nootka")
	set(CMAKE_SKIP_BUILD_RPATH  FALSE) # use, i.e. don't skip the full RPATH for the build tree
	if(OBS_SUSE_QUIRKS)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    message("Build for Suse OBS")
  else(OBS_SUSE_QUIRKS)
    set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) # when building, don't use the install RPATH already (but later on when installing)
  endif(OBS_SUSE_QUIRKS)
	set(CMAKE_INSTALL_RPATH "${NOOTKA_LIB_PATH}")
	# add the automatically determined parts of the RPATH
	# which point to directories outside the build tree to the install RPATH
	set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

	# the RPATH to be used when installing, but only if it's not a system directory
	list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${NOOTKA_LIB_PATH}" isSystemDir)
	if("${isSystemDir}" STREQUAL "-1")
		set(CMAKE_INSTALL_RPATH "${NOOTKA_LIB_PATH}")
	endif("${isSystemDir}" STREQUAL "-1")
endif(UNIX AND NOT APPLE)

if (APPLE) # Mac RPath
  set(CMAKE_MACOSX_RPATH 1)
  set(CMAKE_INSTALL_RPATH "@loader_path/../Frameworks")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif (APPLE)


add_subdirectory( libs/core )     # libNootkaCore
add_subdirectory( libs/score )    # libNootkaScore
add_subdirectory( libs/widgets )  # libNootkaWidgets
add_subdirectory( libs/sound )    # libNootkaSound
add_subdirectory( libs/misc )     # libNootkaMisc

include_directories( libs/core libs/score libs/widgets libs/sound libs/misc )

# nootka executable
set(NOOTKA_SRC
	main.cpp
	mainwindow.cpp

	gui/tmainview.cpp
	gui/tmelman.cpp
	gui/ttoolbar.cpp
	gui/tmenu.cpp
	gui/tstatuslabel.cpp

	guitar/tfingerboard.cpp
	guitar/tguitarview.cpp

	notename/tnotename.cpp
	notename/tnotenamelabel.cpp
	notename/tnametip.cpp

	score/tmultiscore.cpp
	score/tmainscore.cpp
	score/tscoreactions.cpp

	exam/tcanvas.cpp
	exam/tnootkacertificate.cpp
	exam/tquestiontip.cpp
	exam/texamview.cpp
	exam/tprogresswidget.cpp
	exam/tstartexamdlg.cpp
	exam/texamexecutor.cpp
	exam/texecutorsupply.cpp
	exam/texercises.cpp
	exam/tglobalexamstore.cpp
	exam/texamsummary.cpp
	exam/tequalrand.cpp
	exam/trandmelody.cpp
	exam/tpenalty.cpp
	exam/texammelody.cpp

	animations/tstrikedoutitem.cpp
)


if(MINGW)
    add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/nootico.o
                       COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/../picts/nootka-icon.rc
                            -o ${CMAKE_CURRENT_BINARY_DIR}/nootico.o )
    set(NOOTKA_SRCS ${NOOTKA_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/nootico.o)
    set(NOOTKA_EXE_ICON nootico.o)
endif(MINGW)


add_executable(nootka WIN32 ${NOOTKA_SRC} ${NOOTKA_EXE_ICON})
target_link_libraries(nootka
		NootkaCore
		NootkaScore
		NootkaWidgets
		NootkaSound
		NootkaMisc
		Qt5::Widgets
		Qt5::PrintSupport
)


if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
  install(TARGETS nootka DESTINATION bin)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS nootka DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS nootka DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/MacOs")
    install( CODE "
    execute_process(COMMAND echo copying info.plist )
    execute_process(COMMAND sh ${CMAKE_SOURCE_DIR}/packaging/mac/grabInfo.sh ${Nootka_VERSION_FULL} ${CMAKE_SOURCE_DIR} ${CMAKE_INSTALL_PREFIX} )"
    )
  endif(WIN32)
endif(UNIX AND NOT APPLE)

add_subdirectory( plugins )


# get_directory_property( DirDefs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMPILE_DEFINITIONS )
# message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ${DirDefs}" )


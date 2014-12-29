########################################################################
# CMakeList.txt src
########################################################################

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)


find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)

if(UNIX AND NOT APPLE)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro")
    set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-z,relro")
    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,relro")
endif(UNIX AND NOT APPLE)


add_subdirectory( libs/core ) # libNootkaCore
add_subdirectory( libs/sound ) # libNootkaSound
add_subdirectory( libs/misc ) # libNootkaMisc
		
include_directories( libs/core libs/sound libs/misc )

# nootka executable
set(NOOTKA_SRC
	main.cpp
	mainwindow.cpp
	
	gui/tmainview.cpp
	gui/tmelman.cpp
	gui/ttoolbar.cpp
	
	guitar/tfingerboard.cpp
	
	notename/tnotename.cpp
	notename/tnotenamelabel.cpp
	
	score/tmultiscore.cpp
	score/tmainscore.cpp
	score/tcornerproxy.cpp
	score/tscorekeys.cpp
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
    install(TARGETS nootka BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX})
    install( CODE "
    execute_process(COMMAND echo copying info.plist )
    execute_process(COMMAND sh ${CMAKE_SOURCE_DIR}/packaging/mac/grabInfo.sh ${Nootka_VERSION_FULL} ${CMAKE_SOURCE_DIR} ${CMAKE_INSTALL_PREFIX} )"
    )
  endif(WIN32)
endif(UNIX AND NOT APPLE)

add_subdirectory( plugins )

add_subdirectory( updater )

# get_directory_property( DirDefs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMPILE_DEFINITIONS )
# message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ${DirDefs}" )


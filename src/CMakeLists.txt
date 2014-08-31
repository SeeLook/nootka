########################################################################
# CMakeList.txt src
########################################################################

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)


find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
# set(QtCore_location Qt5::Core "" CACHE INTERNAL "" FORCE)
# get_target_property(QtCore_location Qt5::Core LOCATION)

if(UNIX AND NOT APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-z,relro")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,relro")
else(UNIX AND NOT APPLE)
# 	if (WIN32)
# 				set(CMAKE_CXX_FLAGS "-O2 -march=pentium3m") # SoundTouch require it
# 	endif(WIN32)
endif(UNIX AND NOT APPLE)


add_subdirectory( libs/core ) # libNootkaCore
add_subdirectory( libs/sound ) # libNootkaSound
add_subdirectory( libs/misc ) # libNootkaMisc
		
include_directories( libs/core libs/sound libs/misc )

#add_subdirectory( charts ) # nootka analyzer

# add_subdirectory( level ) # level creator

#add_subdirectory( updater ) # nootka updater

add_subdirectory( settings )

# nootka executable
set(NOOTKA_SRC
	main.cpp
	tmainview.cpp
	tmelman.cpp
	mainwindow.cpp
	
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


# get_directory_property( DirDefs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMPILE_DEFINITIONS )
# message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ${DirDefs}" )


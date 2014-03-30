########################################################################
# CMakeList.txt src
########################################################################

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)


find_package(Qt5Widgets)
# set(QtCore_location Qt5::Core "" CACHE INTERNAL "" FORCE)
# get_target_property(QtCore_location Qt5::Core LOCATION)


if(UNIX AND NOT APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-z,relro")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,relro")
else(UNIX AND NOT APPLE)
	if (WIN32)
				set(CMAKE_CXX_FLAGS "-O2 -march=pentium3m") # SoundTouch require it
	endif(WIN32)
endif(UNIX AND NOT APPLE)

# libNootkaCore
add_subdirectory( libs/core )
		
include_directories( libs/core )

# add_subdirectory( charts ) # nootka analyzer

add_subdirectory( level ) # level creator

# add_subdirectory( updater ) # nootka updater

# nootka executable
set(NOOTKA_SRC
	main.cpp
	mainwindow.cpp
	
	guitar/tfingerboard.cpp
	
	score/tscoreview.cpp
	score/tsimplescore.cpp
	score/tmainscore.cpp
	
	animations/tstrikedoutitem.cpp
)


add_executable(nootka WIN32 ${NOOTKA_SRC} )
target_link_libraries(nootka NootkaCore Qt5::Widgets)


# get_directory_property( DirDefs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMPILE_DEFINITIONS )
# message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ${DirDefs}" )


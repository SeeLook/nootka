########################################################################
# CMakeList.txt src
########################################################################

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)


find_package(Qt5Widgets)


# libNootkaCore
add_subdirectory( libs/core )
		
include_directories( libs/core )

add_subdirectory( charts ) # nootka analyzer

add_subdirectory( updater ) # nootka updater


# get_directory_property( DirDefs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMPILE_DEFINITIONS )
# message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ${DirDefs}" )


########################################################################
# CMakeList.txt src
########################################################################

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)


find_package(Qt5Widgets)

# libNootkaCore
	add_subdirectory(libs/core)
		

# get_directory_property( DirDefs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMPILE_DEFINITIONS )
# message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ${DirDefs}" )
		
include_directories( libs/core )

# add_executable(fake libs/main.cpp)
# target_link_libraries(fake NootkaCore Qt5::Widgets)

add_subdirectory( charts )

add_subdirectory(updater)


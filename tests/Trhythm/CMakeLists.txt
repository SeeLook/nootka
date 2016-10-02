########################################################################
# MAIN CMAKE FILE FOR Nootka
########################################################################
 
cmake_minimum_required(VERSION 3.0)

project(trhythm)



# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/tests)
# set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/src)


set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)


find_package(Qt5Core REQUIRED)

# Force using c++11 if not set already - now it is mandatory
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories( ../../src/libs/core/music)

set(TRHYTHM_SRC
  main.cpp
  ../../src/libs/core/music/trhythm.cpp
)

add_executable(trhythm ${TRHYTHM_SRC})
target_link_libraries(trhythm
		Qt5::Core
)

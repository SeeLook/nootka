########################################################################
# bp - (bandoneon parser) parses svg of bandoneon keys layout
########################################################################
 
cmake_minimum_required(VERSION 3.1)

project(bp)

find_package(Qt5Core REQUIRED)
set(CMAKE_AUTOMOC ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../src/)

include_directories( ../../src/libs/core )

set(BP_SRC  main.cpp)
  
add_executable(bp ${BP_SRC})
target_link_libraries(bp NootkaCore Qt5::Core)


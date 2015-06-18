########################################################################
# nootoxml - converts zipped noo files into clear, readable XML format
########################################################################
 
cmake_minimum_required(VERSION 2.8.12)

project(nootoxml)

find_package(Qt5Core REQUIRED)
set(CMAKE_AUTOMOC ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../src/)


set(NOOTOXML_SRC  main.cpp)
  
add_executable(nootoxml ${NOOTOXML_SRC})
target_link_libraries(nootoxml Qt5::Core)


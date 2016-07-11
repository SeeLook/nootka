########################################################################
# scorek - score library developing platform
########################################################################

cmake_minimum_required(VERSION 2.8.12)

project(scorek)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Svg REQUIRED)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../src/)

include_directories( ../../src/libs/core ../../src/libs/score )

set(SCOREK_SRC
  main.cpp
  scorekwindow.cpp

  keylabel.cpp
  keyboardmap.cpp
  pianokey.cpp
  pianokeybd.cpp
  pianoscene.cpp
)

add_executable(scorek ${SCOREK_SRC})
target_link_libraries(scorek NootkaScore Qt5::Widgets Qt5::Svg)


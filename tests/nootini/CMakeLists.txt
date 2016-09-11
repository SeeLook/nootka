########################################################################
# nootini - Nootka research tool for pitch detection
########################################################################
 
cmake_minimum_required(VERSION 2.8.12)

project(nootini)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif (CMAKE_BUILD_TYPE STREQUAL "Release")

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Svg REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../src/)

include_directories( ../../src/libs/core ../../src/libs/score ../../src/libs/sound ../../src/plugins/charts )
if (${WIN32})
  include_directories(${FFTW3_INCLUDE_DIRS})
endif(${WIN32})

set(NOOTINI_SRC
  main.cpp
  nootiniwindow.cpp
  nchart.cpp
  naudioloader.cpp
  nootinisettings.cpp

  ../../src/plugins/charts/tchart.cpp
  ../../src/plugins/charts/tabstractaxis.cpp
  ../../src/plugins/charts/txaxis.cpp
  ../../src/plugins/charts/tyaxis.cpp
  ../../src/plugins/charts/ttiphandler.cpp
  ../../src/plugins/charts/tgroupedqaunit.cpp
  ../../src/plugins/settings/tvolumeslider.cpp

)
  
add_executable(nootini WIN32 ${NOOTINI_SRC})
target_link_libraries(nootini NootkaScore NootkaSound Qt5::Widgets Qt5::Svg)


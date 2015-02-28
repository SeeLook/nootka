########################################################################
# nootini - Nootka research tool for pitch detection
########################################################################
 
cmake_minimum_required(VERSION 2.8.12)

project(nootini)

find_package(Qt5Widgets REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../src/)

include_directories( ../../src/libs/core ../../src/libs/sound ../../src/plugins/charts )

set(NOOTINI_SRC
  main.cpp
  nootiniwindow.cpp
  nchart.cpp
  naudioloader.cpp

  ../../src/plugins/charts/tchart.cpp
  ../../src/plugins/charts/tabstractaxis.cpp
  ../../src/plugins/charts/txaxis.cpp
  ../../src/plugins/charts/tyaxis.cpp
  ../../src/plugins/charts/ttiphandler.cpp

)
  
add_executable(nootini WIN32 ${NOOTINI_SRC})
target_link_libraries(nootini NootkaCore NootkaSound Qt5::Widgets)


########################################################################
# nootest - Nootka plugins tester
########################################################################
 
cmake_minimum_required(VERSION 2.8.12)

project(nootest)

find_package(Qt5Widgets REQUIRED)
set(CMAKE_AUTOMOC ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../src/)

include_directories( ../../src/libs/core )


set(NOOTEST_SRC
  main.cpp
  ../../src/plugins/tplugininterface.h
  ../../src/plugins/tpluginsloader.cpp
  )
  
add_executable(nootest WIN32 ${NOOTEST_SRC})
target_link_libraries(nootest
    NootkaCore 
    NootkaSound 
    NootkaMisc 
    Qt5::Widgets
)


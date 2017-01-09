########################################################################
# scorek - score library developing platform
########################################################################

cmake_minimum_required(VERSION 2.8.12)

project(scorek-lib)

find_package(Qt5Gui REQUIRED)
if (Qt5Gui_FOUND)
    if (Qt5Gui_VERSION VERSION_LESS 5.7.0)
        message(FATAL_ERROR "Minimum supported Qt5 version is 5.7.0")
    endif()
else()
    message(SEND_ERROR "The Qt5Gui library could not be found!")
endif(Qt5Gui_FOUND)

find_package(Qt5Widgets REQUIRED)
find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/../../src/)

include_directories( ../../src/libs/core ../../src/libs/score declarative )

set(SCOREK_SRC
  main.cpp

  declarative/descore.cpp
  declarative/denote.cpp
)

qt5_add_resources(SCOREK_SRC scorek.qrc)

add_executable(scorek-lib ${SCOREK_SRC})
target_link_libraries(scorek-lib 
  NootkaScore
  Qt5::Widgets
  Qt5::Core
  Qt5::Gui
  Qt5::Qml
  Qt5::Quick
  Qt5::QuickControls2
  )


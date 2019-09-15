########################################################################
# CMakeList.txt RtAudio library for Windows build
########################################################################

cmake_minimum_required(VERSION 2.8.12)

project(RtAudio)
# Revert c++11 flag set somewhere above - ASIO won't compile with c++11
set(CMAKE_CXX_FLAGS "")

#message( ${CMAKE_CXX_FLAGS})

set(CMAKE_AUTOMOC ON)
find_package(Qt5Core REQUIRED)

set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}../../../../)

include_directories( ../.  . )

add_definitions(-D__WINDOWS_WASAPI__)
add_definitions(-D__WINDOWS_DS__)
add_definitions(-D__WINDOWS_ASIO__)
add_definitions(-D__WINDOWS_MM__)


set(LIB_RTAUDIO_SRC
  RtAudio.cpp
  RtMidi.cpp
  asio.cpp
  asiodrivers.cpp
  asiolist.cpp
  iasiothiscallresolver.cpp
  ../tasioemitter.h
)

add_library(RtAudio SHARED ${LIB_RTAUDIO_SRC} )
target_link_libraries(RtAudio Qt5::Core ${CMAKE_THREAD_LIBS_INIT} winmm ole32 dsound mfuuid ksuser mfplat wmcodecdspuuid)

install(TARGETS RtAudio DESTINATION .)

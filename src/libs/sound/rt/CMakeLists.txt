########################################################################
# CMakeList.txt RtAudio library for Windows build
########################################################################


# Revert c++11 flag set somewhere above - ASIO won't compile with c++11
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_definitions(-D__WINDOWS_WASAPI__)
add_definitions(-D__WINDOWS_DS__)
add_definitions(-D__WINDOWS_ASIO__)
add_definitions(-D__WINDOWS_MM__)
# set(WIN_LIBS winmm ole32 dsound uuid ksuser) # uuid & ksuser are required by WASAPI


set(LIB_RTAUDIO_SRC
  RtAudio.cpp
  RtMidi.cpp
  asio.cpp
  asiodrivers.cpp
  asiolist.cpp
  iasiothiscallresolver.cpp
)

add_library(RtAudio SHARED ${LIB_RTAUDIO_SRC} )
target_link_libraries(RtAudio winmm ole32 dsound uuid ksuser)

install(TARGETS NootkaSound DESTINATION .)

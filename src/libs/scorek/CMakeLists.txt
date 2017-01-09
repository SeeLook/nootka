########################################################################
# CMakeList.txt Scorek library
########################################################################

add_definitions(-DSCOREK_LIBRARY)

set(LIB_SCOREK_SRC
  music/tnote.cpp
  music/trhythm.cpp
  music/tnamestylefilter.cpp
  music/tclef.cpp
  music/tkeysignature.cpp
  music/tmeter.cpp
)


add_library(scorek SHARED ${LIB_SCOREK_SRC} )
target_link_libraries(scorek 
    Qt5::Widgets
    Qt5::Core
    Qt5::Gui
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
  )


if(UNIX AND NOT APPLE)   # Linux path for Nootka library
  install(TARGETS scorek DESTINATION lib/nootka)
else(UNIX AND NOT APPLE)
  if(WIN32)       # Windows
    install(TARGETS scorek DESTINATION .)
  else(WIN32)       # MacOs
    install(TARGETS scorek DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks")
  endif(WIN32)
endif(UNIX AND NOT APPLE)



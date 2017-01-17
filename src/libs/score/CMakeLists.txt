########################################################################
# CMakeList.txt NootkaScore library
########################################################################

include_directories( . ../core )

add_definitions(-DNOOTKACORE_LIBRARY)

set(LIB_NOOTKASCORE_SRC
#   tscoreplugin.cpp
#   graphics/tnotepixmap.cpp

  declarative/descore.cpp
  declarative/destaff.cpp
  declarative/denote.cpp
  declarative/dekeysignature.cpp

  tscoreitem.cpp
  tscoreclef.cpp
  tscorekeysignature.cpp
  tscorenote.cpp
  tnoteitem.cpp
  tscorescene.cpp
  tscorescordature.cpp
  tscorestaff.cpp
  tscoremeasure.cpp
  tscorebeam.cpp
  tscoretie.cpp
  tscore5lines.cpp
  tscorelines.cpp
  tscoremeter.cpp
)


get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()

add_library(NootkaScore SHARED ${LIB_NOOTKASCORE_SRC} )
target_link_libraries(NootkaScore NootkaCore Qt5::Widgets)


if(UNIX AND NOT APPLE)   # Linux path for Nootka library
  install(TARGETS NootkaScore DESTINATION lib/nootka)
else(UNIX AND NOT APPLE)
  if(WIN32)       # Windows
    install(TARGETS NootkaScore DESTINATION .)
  else(WIN32)       # MacOs
    install(TARGETS NootkaScore DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks")
  endif(WIN32)
endif(UNIX AND NOT APPLE)



########################################################################
# CMakeList.txt nootka ABOUT plugin library
########################################################################


set(ABOUT_SRC
  taboutplugin.cpp
  taboutnootka.cpp
  tsupportnootka.cpp
)


add_library(NootkaAboutPlugin SHARED ${ABOUT_SRC} )
target_link_libraries(NootkaAboutPlugin NootkaCore NootkaMisc Qt5::Widgets)

if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
  install(TARGETS NootkaAboutPlugin DESTINATION lib/nootka)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS NootkaAboutPlugin DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS NootkaAboutPlugin DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks")
  endif(WIN32)
endif(UNIX AND NOT APPLE)

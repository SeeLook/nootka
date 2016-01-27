########################################################################
# CMakeList.txt nootka level plugin library
########################################################################


set(LEVEL_SRC
  tlevelplugin.cpp
  tlevelcreatordlg.cpp
  tlevelheaderwdg.cpp
  tabstractlevelpage.cpp
  accidsettings.cpp
  tkeysigncombobox.cpp
  levelsettings.cpp
  questionssettings.cpp
#   tmelodysettings.cpp
  rangesettings.cpp
)


add_library(NootkaLevelPlugin SHARED ${LEVEL_SRC} )
target_link_libraries(NootkaLevelPlugin NootkaCore NootkaSound Qt5::Widgets)

if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
  install(TARGETS NootkaLevelPlugin DESTINATION lib/nootka)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS NootkaLevelPlugin DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS NootkaLevelPlugin DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks")
  endif(WIN32)
endif(UNIX AND NOT APPLE)

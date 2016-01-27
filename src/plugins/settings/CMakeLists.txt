########################################################################
# CMakeList.txt nootka settings plugin
########################################################################

include_directories( . ../libs/core ../libs/sound/ )
if (WIN32)
  include_directories(${FFTW3_INCLUDE_DIRS})
endif(WIN32)

set(SETTINGS_SRC
  tsettingsplugin.cpp
  tsettingsdialog.cpp
  audioinsettings.cpp
  audiooutsettings.cpp
  tcolorbutton.cpp
  texamsettings.cpp
  tglobalsettings.cpp
  tguitarsettings.cpp
  tlaysettings.cpp
  tnotationradiogroup.cpp
  tnotenamesettings.cpp
  tscoresettings.cpp
  tvolumeslider.cpp
)


add_library(NootkaSettingsPlugin SHARED ${SETTINGS_SRC} )
target_link_libraries(NootkaSettingsPlugin NootkaCore NootkaSound NootkaMisc Qt5::Widgets)


if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
  install(TARGETS NootkaSettingsPlugin DESTINATION lib/nootka)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS NootkaSettingsPlugin DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS NootkaSettingsPlugin DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks")
  endif(WIN32)
endif(UNIX AND NOT APPLE)

########################################################################
# CMakeList.txt nootka-settings
########################################################################

include_directories( . ../libs/core ../libs/sound/ )
if (WIN32)
  include_directories(${FFTW3_INCLUDE_DIRS})
endif(WIN32)

set(SETTINGS_SRC
  main.cpp
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
  tupdateprocess.cpp
  tvolumeslider.cpp
)


add_executable(nootka-settings WIN32 ${SETTINGS_SRC} )
target_link_libraries(nootka-settings NootkaCore NootkaSound NootkaMisc Qt5::Widgets)


if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
  install(TARGETS nootka-settings DESTINATION bin)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS nootka-settings DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS nootka-settings BUNDLE DESTINATION "nootka.app/Contents/MacOS/")
#     install( CODE "
#     execute_process(COMMAND echo copying info.plist )
#     execute_process(COMMAND sh ${CMAKE_SOURCE_DIR}/packaging/mac/grabInfo.sh ${Nootka_VERSION_FULL} ${CMAKE_SOURCE_DIR} ${CMAKE_INSTALL_PREFIX} )"
#     )
  endif(WIN32)
endif(UNIX AND NOT APPLE)

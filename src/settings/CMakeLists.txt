########################################################################
# CMakeList.txt nootka-settings
########################################################################

include_directories( . ../core )

set(SETTINGS_SRC
  main.cpp
  tsettingsdialog.cpp
  audioinsettings.cpp
  audiooutsettings.cpp
  tcolorbutton.cpp
  texamsettings.cpp
  tglobalsettings.cpp
  tguitarsettings.cpp
  tnotationradiogroup.cpp
  tnotenamesettings.cpp
  tscoresettings.cpp
  tupdateprocess.cpp
  tvolumeslider.cpp
)

if(MINGW)
		# dress up win executable in nice nootka icon ONLY FOR MINGW !!!!!!!!!!!!!
		add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/chartico.o
												COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_SOURCE_DIR}/picts/chart-icon.rc
														-o ${CMAKE_CURRENT_BINARY_DIR}/chartico.o )
		set(SETTINGS_SRC ${SETTINGS_SRC} ${CMAKE_CURRENT_BINARY_DIR}/chartico.o)
endif(MINGW)


add_executable(nootka-settings WIN32 ${SETTINGS_SRC} )
target_link_libraries(nootka-settings NootkaCore NootkaSound NootkaMisc Qt5::Widgets)


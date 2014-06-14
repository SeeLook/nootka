########################################################################
# CMakeList.txt nootka-level
########################################################################


set(LEVEL_SRC
  main.cpp
  tlevelcreatordlg.cpp
  tlevelheaderwdg.cpp
  accidsettings.cpp
  tkeysigncombobox.cpp
  levelsettings.cpp
  questionssettings.cpp
  rangesettings.cpp
  tlevelselector.cpp
)

if(MINGW)
		# dress up win executable in nice nootka icon ONLY FOR MINGW !!!!!!!!!!!!!
		add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/chartico.o
												COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_SOURCE_DIR}/picts/chart-icon.rc
														-o ${CMAKE_CURRENT_BINARY_DIR}/chartico.o )
		set(LEVEL_SRC ${LEVEL_SRC} ${CMAKE_CURRENT_BINARY_DIR}/chartico.o)
endif(MINGW)


add_executable(nootka-level WIN32 ${LEVEL_SRC} )
target_link_libraries(nootka-level NootkaCore NootkaSound Qt5::Widgets)


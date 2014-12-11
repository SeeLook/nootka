########################################################################
# CMakeList.txt nootka-level
########################################################################


set(LEVEL_SRC
  main.cpp
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

# if(MINGW)
# 		# dress up win executable in nice nootka icon ONLY FOR MINGW !!!!!!!!!!!!!
# 		add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/chartico.o
# 												COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_SOURCE_DIR}/picts/chart-icon.rc
# 														-o ${CMAKE_CURRENT_BINARY_DIR}/chartico.o )
# 		set(LEVEL_SRC ${LEVEL_SRC} ${CMAKE_CURRENT_BINARY_DIR}/chartico.o)
# endif(MINGW)


add_executable(nootka-level WIN32 ${LEVEL_SRC} )
target_link_libraries(nootka-level NootkaCore NootkaSound Qt5::Widgets)

if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
  install(TARGETS nootka-level DESTINATION bin)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS nootka-level DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS nootka-level BUNDLE DESTINATION "nootka.app/Contents/MacOS/")
#     install( CODE "
#     execute_process(COMMAND echo copying info.plist )
#     execute_process(COMMAND sh ${CMAKE_SOURCE_DIR}/packaging/mac/grabInfo.sh ${Nootka_VERSION_FULL} ${CMAKE_SOURCE_DIR} ${CMAKE_INSTALL_PREFIX} )"
#     )
  endif(WIN32)
endif(UNIX AND NOT APPLE)

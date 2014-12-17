########################################################################
# CMakeList.txt nootka-analyzer
########################################################################


set(ANALYZER_SRC
  main.cpp
	tanalysdialog.cpp
	sorting.cpp
	tabstractaxis.cpp
	taxis.h
	tbar.cpp
	tbarchart.cpp
	tchart.cpp
	tgraphicsline.cpp
	tlinearchart.cpp
	tmainchart.cpp
	tmainline.cpp
	tquestionpoint.cpp
	tstafflinechart.cpp
	tstatisticstip.cpp
	ttipchart.cpp
	ttiphandler.cpp
	txaxis.cpp
	tyaxis.cpp
)


add_executable(nootka-analyzer WIN32 ${ANALYZER_SRC} )
target_link_libraries(nootka-analyzer NootkaCore Qt5::Widgets)

if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
  install(TARGETS nootka-analyzer DESTINATION bin)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS nootka-analyzer DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS nootka-analyzer BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX})
#     install( CODE "
#     execute_process(COMMAND echo copying info.plist )
#     execute_process(COMMAND sh ${CMAKE_SOURCE_DIR}/packaging/mac/grabInfo.sh ${Nootka_VERSION_FULL} ${CMAKE_SOURCE_DIR} ${CMAKE_INSTALL_PREFIX} )"
#     )
  endif(WIN32)
endif(UNIX AND NOT APPLE)

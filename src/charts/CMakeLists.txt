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

if(MINGW)
		# dress up win executable in nice nootka icon ONLY FOR MINGW !!!!!!!!!!!!!
		add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/chartico.o
												COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/../picts/chart-icon.rc
														-o ${CMAKE_CURRENT_BINARY_DIR}/chartico.o )
		set(ANALYZER_SRC ${ANALYZER_SRC} ${CMAKE_CURRENT_BINARY_DIR}/chartico.o)
endif(MINGW)


add_executable(nootka-analyzer WIN32 ${ANALYZER_SRC} )
target_link_libraries(nootka-analyzer NootkaCore Qt5::Widgets)


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


add_executable(nootka-analyzer ${ANALYZER_SRC} )
target_link_libraries(nootka-analyzer NootkaCore Qt5::Widgets)
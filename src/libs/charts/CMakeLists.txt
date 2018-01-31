########################################################################
# CMakeList.txt Nootka analyzer plugin library
########################################################################


set(ANALYZER_SRC
  tanalyzerplugin.cpp
  tanalysdialog.cpp
  sorting.cpp
  tabstractaxis.cpp
  tbar.cpp
  tbarchart.cpp
  tchart.cpp
  tgraphicsline.cpp
  tgroupedqaunit.cpp
  tlinearchart.cpp
  tmainchart.cpp
  tmainline.cpp
  tquestionpoint.cpp
  tstafflinechart.cpp
  tstatisticstip.cpp
  ttipchart.cpp
  ttiphandler.cpp
  ttipmelody.cpp
  tmelodyview.cpp
  txaxis.cpp
  tyaxis.cpp
)


add_library(NootkaAnalyzerPlugin SHARED ${ANALYZER_SRC} )
target_link_libraries(NootkaAnalyzerPlugin NootkaCore Qt5::Widgets)

if(UNIX AND NOT APPLE)   # Linux path for Nootka binary
  install(TARGETS NootkaAnalyzerPlugin DESTINATION lib/nootka)
else(UNIX AND NOT APPLE)
  if(WIN32)       # Windows
    install(TARGETS NootkaAnalyzerPlugin DESTINATION .)
  else(WIN32)       # MacOs
    install(TARGETS NootkaAnalyzerPlugin DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks")
  endif(WIN32)
endif(UNIX AND NOT APPLE)

########################################################################
# CMakeList.txt nootka NootkaExamPlugin library
########################################################################

include_directories( . ../libs/core ../libs/sound/ ../libs/main )

set(EXAM_SRC
  texamplugin.cpp
  tcanvas.cpp
  tnootkacertificate.cpp
  tquestiontip.cpp
  tstartexamdlg.cpp
  texamexecutor.cpp
  texecutorsupply.cpp
  texercises.cpp
  tglobalexamstore.cpp
  texamsummary.cpp
  tequalrand.cpp
  tpenalty.cpp
  texammelody.cpp
)


add_library(NootkaExamPlugin SHARED ${EXAM_SRC} )
target_link_libraries(NootkaExamPlugin NootkaMain Qt5::Widgets Qt5::PrintSupport)

if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
  install(TARGETS NootkaExamPlugin DESTINATION lib/nootka)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS NootkaExamPlugin DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS NootkaExamPlugin DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks")
  endif(WIN32)
endif(UNIX AND NOT APPLE)

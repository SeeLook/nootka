########################################################################
# CMakeList.txt NootkaCore library
########################################################################

add_definitions(-DNOOTKACORE_LIBRARY)

set(LIB_NOOTKACORE_SRC
	tinitcorelib.cpp
	texamparams.h
	taudioparams.h
	tcolor.cpp
	tfingerpos.cpp
	tglobals.cpp
	tnoofont.cpp
	
	animations/tabstractanim.cpp
	animations/tblinkingitem.cpp
	animations/tcoloredanim.cpp
	animations/tcombinedanim.cpp
	animations/tfadeinanim.cpp
	animations/tmorphedanim.cpp
	animations/tmovedanim.cpp
	animations/tscaledanim.cpp
	animations/tcrossfadetextanim.cpp
	
	music/tnote.cpp
	music/tnamestylefilter.cpp
	music/ttune.cpp
	music/tclef.cpp
	music/tinstrument.cpp
	music/tkeysignature.cpp
	
	graphics/tdropshadoweffect.cpp
	graphics/tgraphicstexttip.cpp
	graphics/tnotepixmap.cpp
	
	exam/tqatype.cpp
	exam/tqaunit.cpp
	exam/tgroupedqaunit.cpp
	exam/tlevel.cpp
	exam/texam.cpp
	exam/textrans.h
	
	score/tscoreitem.cpp
	score/tscoreclef.cpp
	score/tscorecontrol.cpp
	score/tscorekeysignature.cpp
	score/tscorenote.cpp
	score/tscorescene.cpp
	score/tscorescordature.cpp
	score/tscorestaff.cpp
	score/tnotecontrol.cpp
	score/tsimplescore.cpp
	
	widgets/tpushbutton.cpp
	widgets/tselectclef.cpp
	widgets/tquestionaswdg.cpp
	widgets/tlevelpreview.cpp
	widgets/tsettingsdialogbase.cpp
	widgets/troundedlabel.cpp
)

add_library(NootkaCore SHARED ${LIB_NOOTKACORE_SRC} )
qt5_use_modules(NootkaCore Widgets)


# get_directory_property( CoreDIrDefs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMPILE_DEFINITIONS )
# message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ${CoreDIrDefs}" )




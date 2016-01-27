########################################################################
# CMakeList.txt nootka level plugin library
########################################################################


set(WIZARD_SRC
  twizardplugin.cpp
  tfirstrunwizard.cpp
)


add_library(NootkaWizardPlugin SHARED ${WIZARD_SRC} )
target_link_libraries(NootkaWizardPlugin NootkaCore NootkaMisc Qt5::Widgets)

if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
  install(TARGETS NootkaWizardPlugin DESTINATION lib/nootka)
else(UNIX AND NOT APPLE)
  if(WIN32) 			# Windows
    install(TARGETS NootkaWizardPlugin DESTINATION .)
  else(WIN32) 			# MacOs
    install(TARGETS NootkaWizardPlugin DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks")
  endif(WIN32)
endif(UNIX AND NOT APPLE)

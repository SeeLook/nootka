########################################################################
# MAIN CMAKE FILE FOR Nootka
########################################################################
 
cmake_minimum_required(VERSION 2.8.12)

project(nootka)

SET(Nootka_VERSION_MAJOR  "1")
SET(Nootka_VERSION_MINOR  "1")
SET(Nootka_VERSION_PATCH  "5-alpha")

SET(Nootka_VERSION       "${Nootka_VERSION_MAJOR}.${Nootka_VERSION_MINOR}")
SET(Nootka_VERSION_FULL  "${Nootka_VERSION}.${Nootka_VERSION_PATCH}")
message(STATUS "
${PROJECT_NAME}-${Nootka_VERSION_FULL}
" )


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/src)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/src) 

if (UNIX AND NOT APPLE) # config with version number is generated once on developer platform
configure_file(${CMAKE_SOURCE_DIR}/src/nootkaconfig.h.in ${CMAKE_SOURCE_DIR}/src/libs/core/nootkaconfig.h)
endif (UNIX AND NOT APPLE)

add_subdirectory(src)

###########################################################################
#INSTALLATION
###########################################################################
if(WIN32)
    set(INS_PREF ".")
else(WIN32)
  if (APPLE)
    set(INS_PREF "nootka.app/Contents/Resources")
  else(APPLE)
    set(INS_PREF "share/nootka")
  endif(APPLE)
endif(WIN32)


if(APPLE)
  install( CODE "
      execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/packaging/mac/install.sh ${CMAKE_BINARY_DIR} ${QT_BINARY_DIR} ${FFTW3_LIBRARIES} ${VORBIS_LIBRARIES} )
  " )
	install(FILES "${CMAKE_SOURCE_DIR}/picts/nootka.icns" DESTINATION "${INS_PREF}") # icns for mac
	install(FILES "${CMAKE_SOURCE_DIR}/picts/nootka-levels.icns" DESTINATION "${INS_PREF}")
	install(FILES "${CMAKE_SOURCE_DIR}/picts/nootka-exam.icns" DESTINATION "${INS_PREF}")
endif(APPLE)


install(FILES changes DESTINATION "${INS_PREF}")
file(GLOB ICONS_PNG "${CMAKE_CURRENT_SOURCE_DIR}/picts/*.png") #png-s for all
install(FILES ${ICONS_PNG} DESTINATION "${INS_PREF}/picts")
install(DIRECTORY fonts DESTINATION "${INS_PREF}")

file(GLOB langs "${CMAKE_CURRENT_SOURCE_DIR}/lang/*.qm")
if(APPLE OR WIN32)
  find_package(Qt5Core)
  get_target_property(QtCore_location_Release Qt5::Core LOCATION_Release)
get_filename_component(QT_BINARY_DIR "${QtCore_location_Release}" DIRECTORY)
	set(QT_TRANSLATIONS_DIR "${QT_BINARY_DIR}/../translations")
	install(FILES gpl DESTINATION "${INS_PREF}")
  LIST(APPEND langs "${QT_TRANSLATIONS_DIR}/qt_cs.qm" "${QT_TRANSLATIONS_DIR}/qtbase_cs.qm"
										"${QT_TRANSLATIONS_DIR}/qt_de.qm" "${QT_TRANSLATIONS_DIR}/qtbase_de.qm"
										"${QT_TRANSLATIONS_DIR}/qt_fr.qm" # "${QT_TRANSLATIONS_DIR}/qtbase_fr.qm"
										"${QT_TRANSLATIONS_DIR}/qt_pl.qm" # "${QT_TRANSLATIONS_DIR}/qtbase_pl.qm"
										"${QT_TRANSLATIONS_DIR}/qt_ru.qm" "${QT_TRANSLATIONS_DIR}/qtbase_ru.qm" )
endif(APPLE OR WIN32)
install(FILES ${langs} DESTINATION "${INS_PREF}/lang")

file(GLOB snds "${CMAKE_CURRENT_SOURCE_DIR}/sounds/*-guitar.ogg")
install(FILES ${snds} DESTINATION "${INS_PREF}/sounds")

if(NOT UNIX) # Windows icons
  install(FILES picts/nootka-exam.ico picts/nootka.ico picts/nootka-icon.rc picts/nootka-levels.ico picts/pack.ico picts/nootka.rc DESTINATION "${INS_PREF}/picts") #ico for windows
#  install(FILES picts/logo.bmp DESTINATION "${INS_PREF}") # logo for installer
  install(FILES packaging/nsis/nootka-utf8.nsi packaging/nsis/fileasoc.nsh DESTINATION "${INS_PREF}" )
endif(NOT UNIX)

# files related with Linux (mime, desktop entry, icons)
if(UNIX AND NOT APPLE)
    install(FILES mime/nootka.desktop DESTINATION share/applications)
#     install(FILES picts/nootka.svg DESTINATION "${INS_PREF}/picts")
#     if (DPKG_CMD) # deb packager doesn't accept symbolic links
			install(FILES picts/nootka.png picts/nootka-level.png picts/nootka-exam.png DESTINATION share/pixmaps)
#     else (DPKG_CMD) # but others do
# 			install(FILES mime/nootka.png mime/levelCreator.png mime/nootka-exam.png DESTINATION share/pixmaps)
			install(FILES gpl DESTINATION "${INS_PREF}") # no gpl for Debian-s - they have copyright instead
#     endif (DPKG_CMD)
    install( CODE "
      execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/mime )
      execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/mime/install-mime.sh ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}/mime ${CMAKE_CURRENT_SOURCE_DIR}/mime)" )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mime/nootka.xml DESTINATION share/mime/packages)
		install( CODE "
      execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/doc )
      execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/packaging/make-chlog.sh ${Nootka_VERSION_FULL} ${CMAKE_CURRENT_BINARY_DIR}/doc/changelog ${CMAKE_CURRENT_SOURCE_DIR})
	" )
  if (DPKG_CMD) # Debian changelog
      install(FILES "${CMAKE_CURRENT_BINARY_DIR}/doc/changelog.gz" DESTINATION share/doc/nootka)
  endif (DPKG_CMD)
	install(FILES copyright DESTINATION share/doc/nootka)
	install(FILES packaging/nootka.1.gz DESTINATION share/man/man1)
	INSTALL(FILES packaging/nootka-updater.1.gz DESTINATION share/man/man1)
endif(UNIX AND NOT APPLE)



###########################################################################
# uninstall target
###########################################################################
configure_file(
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
     "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

  add_custom_target(uninstall
   COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)


add_subdirectory( packaging ) # cpack rules


# Uncomment below lines if you want to build any testing executable
# add_subdirectory( tests/plugtester )



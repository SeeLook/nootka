########################################################################
# MAIN CMAKE FILE FOR Nootka
########################################################################

cmake_minimum_required(VERSION 2.8.12)

project(nootka)

SET(Nootka_VERSION_MAJOR  "1")
SET(Nootka_VERSION_MINOR  "4")
SET(Nootka_VERSION_PATCH  "3")

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

if (APPLE)
  install( CODE "
    execute_process(
      COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/nootka.app/Contents)
    ")
endif (APPLE)

add_subdirectory(src)

###########################################################################
#INSTALLATION
###########################################################################
if(WIN32)
  set(PACKAGE_ARCHITECTURE x86)
endif(WIN32)
if(APPLE)
  set(PACKAGE_ARCHITECTURE x86_64)
endif(APPLE)

if(NOT PACKAGE_ARCHITECTURE)
   find_program(DPKG_CMD dpkg)
   if(NOT DPKG_CMD)
      find_program(RPM_CMD rpm)
      if(NOT RPM_CMD)
#         message(STATUS "Can not find any program to determine architecture, default to i386.")
#         set(PACKAGE_ARCHITECTURE i386)
        execute_process(COMMAND uname -m
          OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
          OUTPUT_STRIP_TRAILING_WHITESPACE
        )
      else(NOT RPM_CMD)
        execute_process(COMMAND "${RPM_CMD}" --eval %_target_cpu
          OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
          OUTPUT_STRIP_TRAILING_WHITESPACE 
        )
        set(RPM_CMD_FOUND TRUE)
      endif(NOT RPM_CMD)
   else(NOT DPKG_CMD)
      execute_process(COMMAND "${DPKG_CMD}" --print-architecture
        OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
        OUTPUT_STRIP_TRAILING_WHITESPACE
      )
      add_definitions(-D_FORTIFY_SOURCE=2) # for lintian DEB_BUILD_HARDENING_FORTIFY
   endif(NOT DPKG_CMD)
endif(NOT PACKAGE_ARCHITECTURE)
message(STATUS "Architecture is ${PACKAGE_ARCHITECTURE}")
if (NOT DPKG_CMD)
    unset(DPKG_CMD)
endif (NOT DPKG_CMD)

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
      execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/packaging/mac/install.sh ${CMAKE_INSTALL_PREFIX} ${SOUNDTOUCH_LIBRARY} ${FFTW3_LIBRARIES} ${VORBIS_LIBRARIES} )
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
  if (WIN32)
    set(QT_TRANSLATIONS_DIR "${QT_BINARY_DIR}/../translations")
  else (WIN32)
    set(QT_TRANSLATIONS_DIR "${QT_BINARY_DIR}/../../translations")
  endif (WIN32)
	install(FILES gpl DESTINATION "${INS_PREF}")
 LIST(APPEND langs
   "${QT_TRANSLATIONS_DIR}/qtbase_cs.qm"
   "${QT_TRANSLATIONS_DIR}/qtbase_de.qm"
   "${QT_TRANSLATIONS_DIR}/qtbase_fr.qm"
   "${QT_TRANSLATIONS_DIR}/qtbase_pl.qm"
   "${QT_TRANSLATIONS_DIR}/qtbase_ru.qm"
      # So far not provided by Qt
      #"${CMAKE_CURRENT_SOURCE_DIR}/lang/qtbase/qtbase_es.qm"

  )
endif(APPLE OR WIN32)

install(FILES ${langs} DESTINATION "${INS_PREF}/lang")

if(APPLE)
  file(GLOB mimeTrans "${CMAKE_CURRENT_SOURCE_DIR}/packaging/mac/*.lproj" )
  install(DIRECTORY ${mimeTrans} DESTINATION ${INS_PREF})
endif(APPLE)


file(GLOB snds "${CMAKE_CURRENT_SOURCE_DIR}/sounds/*-guitar.ogg")
install(FILES ${snds} DESTINATION "${INS_PREF}/sounds")

# files related with Linux (mime, desktop entry, icons)
if(UNIX AND NOT APPLE)
    install(FILES mime/nootka.desktop DESTINATION share/applications)
    if (DPKG_CMD) # deb packager doesn't accept symbolic links
			install(FILES picts/nootka.png picts/nootka-level.png picts/nootka-exam.png DESTINATION share/pixmaps)
    else (DPKG_CMD) # but others do
			install(FILES mime/nootka.png mime/nootka-level.png mime/nootka-exam.png DESTINATION share/pixmaps)
			install(FILES gpl DESTINATION "${INS_PREF}") # no gpl for Debian-s - they have copyright instead
    endif (DPKG_CMD)
    install( CODE "
      execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/mime )
      execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/mime/install-mime.sh ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}/mime ${CMAKE_CURRENT_SOURCE_DIR}/mime)" )
    install(FILES ${CMAKE_CURRENT_BINARY_DIR}/mime/nootka.xml DESTINATION share/mime/packages)
		install( CODE "
      execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/doc )
      execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/packaging/make-chlog.sh ${Nootka_VERSION_FULL} ${CMAKE_CURRENT_BINARY_DIR}/doc/changelog ${CMAKE_CURRENT_SOURCE_DIR})
	" )
  if (DPKG_CMD) # Debian changelog
      install(FILES "${CMAKE_CURRENT_BINARY_DIR}/doc/changelog.Debian.gz" DESTINATION share/doc/nootka RENAME changelog.gz)
  endif (DPKG_CMD)
	install(FILES copyright DESTINATION share/doc/nootka)
	install(FILES packaging/nootka.1.gz DESTINATION share/man/man1)
endif(UNIX AND NOT APPLE)

if(WIN32)
  get_filename_component(VORBIS_DIR "${VORBIS_LIBRARIES}" DIRECTORY)
  get_filename_component(MAKE_DIR "${CMAKE_RC_COMPILER}" DIRECTORY) # it works ether with make and ninja
  install(FILES picts/nootka-exam.ico picts/nootka.ico picts/nootka-levels.ico picts/pack.ico picts/logo.bmp picts/logo-left.bmp DESTINATION "${INS_PREF}/picts") #ico
  install(FILES packaging/nsis/nootka-utf8.nsi packaging/nsis/fileasoc.nsh ${CMAKE_BINARY_DIR}/nsis/NSIS.definitions.nsh DESTINATION "${INS_PREF}")
  install(FILES "${FFTW3_LIBRARIES}" "${VORBIS_LIBRARIES}" "${VORBIS_DIR}/libvorbis.dll" "${VORBIS_DIR}/libogg.dll" DESTINATION "${INS_PREF}")
endif(WIN32)

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
# add_subdirectory( tests/nootini )
# add_subdirectory( tests/nootoxml )



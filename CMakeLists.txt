########################################################################
# MAIN CMAKE FILE FOR Nootka
########################################################################

cmake_minimum_required(VERSION 3.1.0)

if (POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

project(nootka)

set(Nootka_VERSION_MAJOR  "1")
set(Nootka_VERSION_MINOR  "7")
set(Nootka_VERSION_PATCH  "3-beta3")


set(Nootka_VERSION       "${Nootka_VERSION_MAJOR}.${Nootka_VERSION_MINOR}")
set(Nootka_VERSION_FULL  "${Nootka_VERSION}.${Nootka_VERSION_PATCH}")
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

add_subdirectory(src)

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
  install(FILES "${CMAKE_SOURCE_DIR}/picts/nootka.icns" DESTINATION "${INS_PREF}") # icns for mac
  install(FILES "${CMAKE_SOURCE_DIR}/picts/nootka-levels.icns" DESTINATION "${INS_PREF}")
  install(FILES "${CMAKE_SOURCE_DIR}/picts/nootka-exam.icns" DESTINATION "${INS_PREF}")
  install(FILES "${CMAKE_SOURCE_DIR}/packaging/mac/qt.conf" DESTINATION "${INS_PREF}")
endif(APPLE)


install(FILES changes DESTINATION "${INS_PREF}")
file(GLOB ICONS_PNG "${CMAKE_CURRENT_SOURCE_DIR}/picts/*.png") #png-s for all
install(FILES ${ICONS_PNG} DESTINATION "${INS_PREF}/picts")
install(DIRECTORY picts/flags DESTINATION "${INS_PREF}/picts")
install(DIRECTORY picts/pane DESTINATION "${INS_PREF}/picts")
install(DIRECTORY fonts DESTINATION "${INS_PREF}")

install(DIRECTORY "levels/" DESTINATION "${INS_PREF}/levels")

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
   "${QT_TRANSLATIONS_DIR}/qtbase_es.qm"
   "${QT_TRANSLATIONS_DIR}/qtbase_fr.qm"
   "${QT_TRANSLATIONS_DIR}/qtbase_hu.qm"
   "${QT_TRANSLATIONS_DIR}/qtbase_it.qm"
   "${QT_TRANSLATIONS_DIR}/qtbase_pl.qm"
   "${QT_TRANSLATIONS_DIR}/qtbase_ru.qm"
#    "${QT_TRANSLATIONS_DIR}/qtbase_sl.qm"
      # So far not provided by Qt
      #"${CMAKE_CURRENT_SOURCE_DIR}/lang/qtbase/qtbase_sl.qm"

  )
endif(APPLE OR WIN32)

install(FILES ${langs} DESTINATION "${INS_PREF}/lang")

if(APPLE)
  file(GLOB MIME_TRANS "${CMAKE_CURRENT_SOURCE_DIR}/packaging/mac/*.lproj" )
  install(DIRECTORY ${MIME_TRANS} DESTINATION ${INS_PREF})
endif(APPLE)


file(GLOB snds "${CMAKE_CURRENT_SOURCE_DIR}/sounds/*.ogg")
list(APPEND snds "${CMAKE_CURRENT_SOURCE_DIR}/sounds/beat.raw48-16")
install(FILES ${snds} DESTINATION "${INS_PREF}/sounds")

# files related with Linux (mime, desktop entry, icons)
if(UNIX AND NOT APPLE)
    install(FILES mime/nootka.desktop DESTINATION share/applications)
    install(FILES mime/nootka.appdata.xml DESTINATION share/metainfo)
    install(FILES mime/nootka.xml DESTINATION share/mime/packages)
    if (NOT DPKG_CMD) # deb packager doesn't accept symbolic links
      install(FILES gpl DESTINATION "${INS_PREF}") # no gpl for Debian-s - they have copyright instead
    endif (NOT DPKG_CMD)
  # Icons in standard location
    install(FILES picts/hicolor/16x16/apps/nootka.png DESTINATION share/icons/hicolor/16x16/apps/)
    install(FILES picts/hicolor/24x24/apps/nootka.png DESTINATION share/icons/hicolor/24x24/apps/)
    install(FILES picts/hicolor/32x32/apps/nootka.png DESTINATION share/icons/hicolor/32x32/apps/)
    install(FILES picts/hicolor/48x48/apps/nootka.png DESTINATION share/icons/hicolor/48x48/apps/)
    install(FILES picts/hicolor/64x64/apps/nootka.png DESTINATION share/icons/hicolor/64x64/apps/)
    install(FILES picts/hicolor/128x128/apps/nootka.png DESTINATION share/icons/hicolor/128x128/apps/)
    install(FILES picts/hicolor/256x256/apps/nootka.png DESTINATION share/icons/hicolor/256x256/apps/)
    install(FILES picts/nootka.png DESTINATION share/icons/hicolor/512x512/apps/)
    install(FILES picts/hicolor/16x16/mimetypes/nootkaexam.png DESTINATION share/icons/hicolor/16x16/mimetypes/)
    install(FILES picts/hicolor/24x24/mimetypes/nootkaexam.png DESTINATION share/icons/hicolor/24x24/mimetypes/)
    install(FILES picts/hicolor/32x32/mimetypes/nootkaexam.png DESTINATION share/icons/hicolor/32x32/mimetypes/)
    install(FILES picts/hicolor/48x48/mimetypes/nootkaexam.png DESTINATION share/icons/hicolor/48x48/mimetypes/)
    install(FILES picts/hicolor/64x64/mimetypes/nootkaexam.png DESTINATION share/icons/hicolor/64x64/mimetypes/)
    install(FILES picts/hicolor/128x128/mimetypes/nootkaexam.png DESTINATION share/icons/hicolor/128x128/mimetypes/)
    install(FILES picts/nootka-exam.png DESTINATION share/icons/hicolor/256x256/mimetypes/ RENAME nootkaexam.png)
    install(FILES picts/hicolor/512x512/mimetypes/nootkaexam.png DESTINATION share/icons/hicolor/512x512/mimetypes/)
    install(FILES picts/hicolor/16x16/mimetypes/nootkalevel.png DESTINATION share/icons/hicolor/16x16/mimetypes/)
    install(FILES picts/hicolor/24x24/mimetypes/nootkalevel.png DESTINATION share/icons/hicolor/24x24/mimetypes/)
    install(FILES picts/hicolor/32x32/mimetypes/nootkalevel.png DESTINATION share/icons/hicolor/32x32/mimetypes/)
    install(FILES picts/hicolor/48x48/mimetypes/nootkalevel.png DESTINATION share/icons/hicolor/48x48/mimetypes/)
    install(FILES picts/hicolor/64x64/mimetypes/nootkalevel.png DESTINATION share/icons/hicolor/64x64/mimetypes/)
    install(FILES picts/hicolor/128x128/mimetypes/nootkalevel.png DESTINATION share/icons/hicolor/128x128/mimetypes/)
    install(FILES picts/nootka-level.png DESTINATION share/icons/hicolor/256x256/mimetypes/ RENAME nootkalevel.png)
    install(FILES picts/hicolor/512x512/mimetypes/nootkalevel.png DESTINATION share/icons/hicolor/512x512/mimetypes/)
    install( CODE "
      execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/doc )
      execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/packaging/make-chlog.sh ${Nootka_VERSION_FULL} ${CMAKE_CURRENT_BINARY_DIR}/doc/changelog ${CMAKE_CURRENT_SOURCE_DIR})
  " )
  if (DPKG_CMD) # Debian changelog
      install(FILES "${CMAKE_CURRENT_BINARY_DIR}/doc/changelog.Debian.gz" DESTINATION share/doc/nootka)
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
  install(FILES packaging/nsis/qt.conf DESTINATION "${INS_PREF}")
  install(FILES "${MAKE_DIR}/libgcc_s_dw2-1.dll" "${MAKE_DIR}/libstdc++-6.dll" "${MAKE_DIR}/libwinpthread-1.dll" DESTINATION "${INS_PREF}")
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
# add_subdirectory( tests/nootoxml )
# add_subdirectory( tests/bp )



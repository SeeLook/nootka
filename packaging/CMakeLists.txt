##########################################################################################
# CMakeList.txt packaging
##########################################################################################

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Application to help with learning classical score notation")
set(CPACK_PACKAGE_VENDOR "Nootka")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/packaging/nootka-desc")
set(CPACK_RESOURCE_FILE_LICENSE    "${PROJECT_SOURCE_DIR}/gpl")

set(CPACK_PACKAGE_VERSION_MAJOR ${Nootka_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${Nootka_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${Nootka_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${Nootka_VERSION_FULL})

set(CPACK_PACKAGE_CONTACT "See Look <seelook@gmail.com>")

set(CPACK_PACKAGE_NAME "nootka")
set(CPACK_PACKAGE_FILE_NAME "nootka_${CPACK_PACKAGE_VERSION}_${PACKAGE_ARCHITECTURE}")


##########################################################################################
#    cmake/cpack  DEB  generator 
##########################################################################################
set(CPACK_DEBIAN_PACKAGE_NAME "nootka") #default CPACK_PACKAGE_NAME
# message(STATUS ${CPACK_DEBIAN_PACKAGE_NAME} )
# CPACK_DEBIAN_PACKAGE_VERSION #CPACK_PACKAGE_VERSION
# PACKAGE_ARCHITECTURE #dpkg --print-architecture or i386
# set(CPACK_DEBIAN_PACKAGE_NAME "nootka" )


if (ENABLE_PULSEAUDIO)
  set(DEB_DEPENDS "libpulse0,")
endif (ENABLE_PULSEAUDIO)

# from Ubuntu 18.04 (Bionic)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.27), libqt5gui5 (>= 5.9), libqt5widgets5, libqt5printsupport5, libqt5network5, qttranslations5-l10n, qml-module-qtquick-controls, qml-module-qtquick-controls2, qml-module-qtquick-dialogs, ${DEB_DEPENDS} libfftw3-single3, libvorbisfile3, libsoundtouch1")
#CPACK_DEBIAN_PACKAGE_MAINTAINER #CPACK_PACKAGE_CONTACT
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Application helps understand the basics of music notation, reading
 and practicing playing musical scores.
 ​Long story short: the user plays notes (melody) displayed by the app,
 which then in real time checks if the note was played correctly.
 .
 Features:
 .
 interactive interface to discover the rules of musical notation
 exercises with possibility to create own sets
 importing melodies from musicxml, using them in exercises
 accurate method for detecting sung and played sounds
 support for different instruments (guitars, piano, sax, bandoneon)
 clefs (treble, bass and others) and grand staff
 analyze of results - charts with exercise details
 .
 https://nootka.sourceforge.io") #CPACK_PACKAGE_DESCRIPTION_SUMMARY
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
# CPACK_DEBIAN_PACKAGE_PRIORITY #optional
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://nootka.sourceforge.io")
# set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "timidity")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/debian/postrm" )


##########################################################################################
#      Resume              ##########################################################
##########################################################################################

if (WIN32)
  set(CPACK_GENERATOR  "NSIS;ZIP")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "${CMAKE_SOURCE_DIR}/picts/pack.ico")
  
else (WIN32)
  if (NOT APPLE)
    set(CPACK_STRIP_FILES "nootka")
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEB "on")
  endif (NOT APPLE)
endif (WIN32)

include (CPack)

###########################################################################
#  packaging targets (make src)
###########################################################################
if(UNIX AND NOT APPLE)
  message(STATUS "
  To build source package, call:
  make src
  ")

  add_custom_target(src
  COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/src/make-src.sh ${Nootka_VERSION_FULL} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}
   )
ENDIF(UNIX AND NOT APPLE)

###########################################################################
#  installing target (make runinplace) - copy or link necessary files to run Nootka without install
###########################################################################
message(STATUS "
  To enable launching nootka without installing, call:
  make runinplace
  Binary nootka is in src folder, so launch it invoking: ./src/nootka
  ")


find_package(Qt5Core)
get_target_property(QtCore_location_Release Qt5::Core LOCATION_Release)
get_filename_component(QT_BINARY_DIR "${QtCore_location_Release}" DIRECTORY)

if(WIN32)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/nsis/definitions.nsh.in
    ${CMAKE_BINARY_DIR}/nsis/NSIS.definitions.nsh
  )
  
  get_filename_component(VORBIS_DIR "${VORBIS_LIBRARIES}" DIRECTORY)
  get_filename_component(MAKE_DIR "${CMAKE_RC_COMPILER}" DIRECTORY)

  add_custom_target(deploy
    COMMAND echo "deploying..."
    COMMAND ${QT_BINARY_DIR}/windeployqt.exe ${CMAKE_INSTALL_PREFIX}/nootka.exe ${CMAKE_INSTALL_PREFIX}/libNootkaSound.dll --release --no-translations --no-svg --qmldir "${CMAKE_SOURCE_DIR}/src/qml"
  )

  add_custom_target(runinplace
    COMMAND echo
    COMMAND echo Copying libraries...
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_BINARY_DIR}/Qt5Core.dll ${CMAKE_BINARY_DIR}/src/.
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_BINARY_DIR}/Qt5Gui.dll ${CMAKE_BINARY_DIR}/src/.
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_BINARY_DIR}/Qt5Widgets.dll ${CMAKE_BINARY_DIR}/src/.
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_BINARY_DIR}/Qt5Network.dll ${CMAKE_BINARY_DIR}/src/.
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_BINARY_DIR}/Qt5PrintSupport.dll ${CMAKE_BINARY_DIR}/src/.
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/src/plugins/platforms
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_BINARY_DIR}/../plugins/platforms/qwindows.dll ${CMAKE_BINARY_DIR}/src/plugins/platforms/.
    
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FFTW3_LIBRARIES} ${CMAKE_BINARY_DIR}/src/.
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VORBIS_LIBRARIES} ${CMAKE_BINARY_DIR}/src/. #libvorbisfile.dll
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VORBIS_DIR}/libvorbis.dll ${CMAKE_BINARY_DIR}/src/.
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VORBIS_DIR}/libogg.dll ${CMAKE_BINARY_DIR}/src/.
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MAKE_DIR}/libgcc_s_dw2-1.dll ${CMAKE_BINARY_DIR}/src/.
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MAKE_DIR}/libstdc++-6.dll ${CMAKE_BINARY_DIR}/src/.
    COMMAND echo Copying files...
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/gpl ${CMAKE_BINARY_DIR}/src/.
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/changes ${CMAKE_BINARY_DIR}/src/.
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/src/fonts
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_SOURCE_DIR}/fonts/nootka.ttf ${CMAKE_BINARY_DIR}/src/fonts/.
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/src/picts
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/picts ${CMAKE_BINARY_DIR}/src/picts/.
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/src/lang
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/lang ${CMAKE_BINARY_DIR}/src/lang/.
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/src/sounds
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/sounds ${CMAKE_BINARY_DIR}/src/sounds/.
    COMMAND echo
   )
ELSE(WIN32)
  if (APPLE)
    set(LINK_PATH "${CMAKE_BINARY_DIR}/Resources")
  else (APPLE)
    set(LINK_PATH "${CMAKE_BINARY_DIR}")
  endif (APPLE)
  add_custom_target(runinplace
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/make-noinstall.sh ${CMAKE_SOURCE_DIR} ${LINK_PATH}
  )
ENDIF(WIN32)

if (APPLE)
  add_custom_target(deploy
    COMMAND echo "deploying..."
    COMMAND echo ${QT_BINARY_DIR}/../../bin
    COMMAND ${QT_BINARY_DIR}/../../bin/macdeployqt ${CMAKE_INSTALL_PREFIX}/nootka.app -qmldir=${CMAKE_SOURCE_DIR}/src/qml
  )

  add_custom_target(dmg
    COMMAND echo "creating dmg image..."
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/mac/create-dmg.sh ${Nootka_VERSION_FULL} ${CMAKE_SOURCE_DIR} ${CMAKE_INSTALL_PREFIX}
  )
endif(APPLE)

if (UNIX AND NOT APPLE)
  # appimage target
  # To correctly generate AppImage set install prefix to '/usr'
  # and when using with older Linux system (i.e. Ubuntu Trusty 14.04)
  # call
  # cmake with -DQT_QMAKE_EXECUTABLE=/opt/qtXX/bin/qmake
  add_custom_target(appimage
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/appimage/make-appimage.sh ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${QT_QMAKE_EXECUTABLE} ${Nootka_VERSION_FULL}
  )
endif()

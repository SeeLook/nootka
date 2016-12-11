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
#		cmake/cpack	DEB  generator 
##########################################################################################
set(CPACK_DEBIAN_PACKAGE_NAME "nootka") #default CPACK_PACKAGE_NAME
# message(STATUS ${CPACK_DEBIAN_PACKAGE_NAME} )
# CPACK_DEBIAN_PACKAGE_VERSION #CPACK_PACKAGE_VERSION
# PACKAGE_ARCHITECTURE #dpkg --print-architecture or i386
# set(CPACK_DEBIAN_PACKAGE_NAME "nootka" )


if (ENABLE_PULSEAUDIO)
	set(DEB_DEPENDS "libpulse0,")
endif (ENABLE_PULSEAUDIO)


# Old Ubuntu (and derivatives) package names (before 16.04)
# set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.19), libqt5gui5, libqt5widgets5, libqt5printsupport5, libqt5network5, qttranslations5-l10n, ${DEB_DEPENDS} libfftw3-3, libvorbisfile3, libsoundtouch0")
# new package names (from Ubuntu 16.04) for libsoundtouch and libfftw
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.19), libqt5gui5, libqt5widgets5, libqt5printsupport5, libqt5network5, qttranslations5-l10n, ${DEB_DEPENDS} libfftw3-single3, libvorbisfile3, libsoundtouch1")
#CPACK_DEBIAN_PACKAGE_MAINTAINER #CPACK_PACKAGE_CONTACT
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Application to learn classical score notation.
 It helps to understand the rules of reading and writing scores
 and helps with developing skills of playing and singing notes.
 .
 Features:
 .
 interactive score to discover and learn how musical notation works
 natural sound of classical guitar
 accurate pitch detection method
 support for different name-callings of the notes
 exams and exercises with different levels
 levels creator with wide range of settings
 numerous settings to personalize for user needs and capabilities
 .
 http://nootka.sf.net") #CPACK_PACKAGE_DESCRIPTION_SUMMARY
set(CPACK_DEBIAN_PACKAGE_SECTION "science")
# CPACK_DEBIAN_PACKAGE_PRIORITY #optional
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://nootka.sf.net")
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "timidity")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/debian/postrm" )


##########################################################################################
#		cmake/cpack	RPM  generator 
##########################################################################################
# CPACK_RPM_PACKAGE_SUMMARY #CPACK_PACKAGE_DESCRIPTION_SUMMARY
set(CPACK_RPM_PACKAGE_NAME "nootka")# Default   : CPACK_PACKAGE_NAME
# CPACK_RPM_PACKAGE_VERSION # Default   : CPACK_PACKAGE_VERSION
# CPACK_RPM_PACKAGE_ARCHITECTURE # Default   : -
set(CPACK_RPM_PACKAGE_RELEASE "3")
set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Education")
set(CPACK_RPM_PACKAGE_VENDOR "Nootka")
set(CPACK_RPM_PACKAGE_URL "http://nootka.sf.net")
# CPACK_RPM_PACKAGE_DESCRIPTION # Default   : CPACK_PACKAGE_DESCRIPTION_FILE if set or "no package description available"
# CPACK_RPM_COMPRESSION_TYPE #  Possible value are: lzma, xz, bzip2 and gzip.
# set(CPACK_RPM_PACKAGE_REQUIRES "libQtGui >= 4.6, qt-devel, fftw-3")
#Mandriva libfftw3
#Suse fftw3 libqt4-devel
#Fedora fftw qt-devel
#CentOs fftw
set(CPACK_RPM_PACKAGE_SUGGESTS "timidity")
# set(CPACK_RPM_SPEC_INSTALL_POST "/usr/bin/update-desktop-database &> /dev/null || :
# /usr/bin/update-mime-database %{_datadir}/mime &> /dev/null || :")
# CPACK_RPM_SPEC_MORE_DEFINE # May be used to add any %define lines to the generated spec file.
# CPACK_RPM_PACKAGE_DEBUG # cpack -D CPACK_RPM_PACKAGE_DEBUG=1 -G RPM
set(CPACK_RPM_USER_BINARY_SPECFILE "${CMAKE_CURRENT_SOURCE_DIR}/rpm/nootka.spec.in")
#             Mandatory : NO
#             Default   : -
#             May be set by the user in order to specify a USER binary spec file
#             to be used by CPackRPM instead of generating the file.
#             The specified file will be processed by CONFIGURE_FILE( @ONLY).
#          CPACK_RPM_GENERATE_USER_BINARY_SPECFILE_TEMPLATE
#             Mandatory : NO
#             Default   : -
#             If set CPack will generate a template for USER specified binary
#             spec file and stop with an error. For example launch CPack like this
#             cpack -D CPACK_RPM_GENERATE_USER_BINARY_SPECFILE_TEMPLATE=1 -G RPM
#             The user may then use this file in order to hand-craft is own
#             binary spec file which may be used with CPACK_RPM_USER_BINARY_SPECFILE.
#          CPACK_RPM_PRE_INSTALL_SCRIPT_FILE
#          CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE
#             Mandatory : NO
#             Default   : -
#             May be used to embbed a pre (un)installation script in the spec file.
#             The refered script file(s) will be read and directly
#             put after the %pre or %preun section
#             One may verify which scriptlet has been included with
#              rpm -qp --scripts  package.rpm
set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpm/rpm-post.sh")
set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/rpm/rpm-post.sh")
#             Mandatory : NO
#             Default   : -
#             May be used to embbed a post (un)installation script in the spec file.
#             The refered script file(s) will be read and directly
#             put after the %post or %postun section
#             One may verify which scriptlet has been included with
#              rpm -qp --scripts  package.rpm
set(CPACK_RPM_CHANGELOG_FILE "${CMAKE_BINARY_DIR}/doc/changelog-rpm")



##########################################################################################
#			Resume              ##########################################################
##########################################################################################

if (WIN32)
  set(CPACK_GENERATOR  "NSIS;ZIP")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "${CMAKE_SOURCE_DIR}/picts/pack.ico")
  
else (WIN32)
  if (NOT APPLE)
    set(CPACK_STRIP_FILES "nootka")
    set(CPACK_GENERATOR "TGZ;RPM;DEB")
    set(CPACK_DEB "on")
  else (NOT APPLE)
    set(CPACK_GENERATOR "OSXX11")
#     set(CPACK)
  endif (NOT APPLE)
endif (WIN32)

include (CPack)

###########################################################################
#	packaging targets (make src)
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
#	installing target (make runinplace) - copy or link necessary files to run nootka without install
###########################################################################
message(STATUS "
	To enable launching nootka without installing, call:
	make runinplace
	Binary nootka is in src folder, so launch it invoking: ./src/nootka
	")


if(WIN32)
#   set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/nsis/Modules/")
#   LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/packaging/nsis/Modules")
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/nsis/definitions.nsh.in
    ${CMAKE_BINARY_DIR}/nsis/NSIS.definitions.nsh
  )
#   configure_file(
#     ${CMAKE_CURRENT_SOURCE_DIR}/nsis/fileasoc.nsh
#     ${CMAKE_BINARY_DIR}/nsis/fileasoc.nsh
#   )
#   configure_file(
#     ${CMAKE_SOURCE_DIR}/picts/pack.ico
#     ${CMAKE_BINARY_DIR}/nsis/pack.ico
#   )
  
	get_filename_component(VORBIS_DIR "${VORBIS_LIBRARIES}" DIRECTORY)
	get_filename_component(MAKE_DIR "${CMAKE_RC_COMPILER}" DIRECTORY)
	find_package(Qt5Core)
	get_target_property(QtCore_location_Release Qt5::Core LOCATION_Release)
	get_filename_component(QT_BINARY_DIR "${QtCore_location_Release}" DIRECTORY)
	file(GLOB ICU_DLLS "${QT_BINARY_DIR}/icu*.dll") # icudtXX.dll icuinXX.dll icuucXX.dll

	add_custom_target(deploy
    COMMAND echo "deploying..."
    COMMAND ${QT_BINARY_DIR}/windeployqt.exe ${CMAKE_INSTALL_PREFIX}/nootka.exe ${CMAKE_INSTALL_PREFIX}/libNootkaUpdaterPlugin.dll ${CMAKE_INSTALL_PREFIX}/libNootkaSound.dll --release --no-translations --no-svg
	)

	add_custom_target(runinplace
		COMMAND echo
		COMMAND echo Copying libraries...
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_BINARY_DIR}/Qt5Core.dll ${CMAKE_BINARY_DIR}/src/.
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_BINARY_DIR}/Qt5Gui.dll ${CMAKE_BINARY_DIR}/src/.
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_BINARY_DIR}/Qt5Widgets.dll ${CMAKE_BINARY_DIR}/src/.
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_BINARY_DIR}/Qt5Network.dll ${CMAKE_BINARY_DIR}/src/.
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_BINARY_DIR}/Qt5PrintSupport.dll ${CMAKE_BINARY_DIR}/src/.
# 		foreach(ICU ${ICU_DLLS})
			COMMAND ${CMAKE_COMMAND} -E copy_if_different ${ICU_DLLS} ${CMAKE_BINARY_DIR}/src/.
# 		endforeach()
		COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/src/plugins/platforms
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${QT_BINARY_DIR}/../plugins/platforms/qwindows.dll ${CMAKE_BINARY_DIR}/src/plugins/platforms/.
		
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${FFTW3_LIBRARIES} ${CMAKE_BINARY_DIR}/src/.
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VORBIS_LIBRARIES} ${CMAKE_BINARY_DIR}/src/. #libvorbisfile.dll
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VORBIS_DIR}/libvorbis.dll ${CMAKE_BINARY_DIR}/src/.
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${VORBIS_DIR}/libogg.dll ${CMAKE_BINARY_DIR}/src/.
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MAKE_DIR}/libgcc_s_dw2-1.dll ${CMAKE_BINARY_DIR}/src/.
		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MAKE_DIR}/libstdc++-6.dll ${CMAKE_BINARY_DIR}/src/.
# 		COMMAND ${CMAKE_COMMAND} -E copy_if_different ${MAKE_DIR}/mingwm10.dll ${CMAKE_BINARY_DIR}/src/.
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
  find_package(Qt5Core)
  get_target_property(QtCore_location_Release Qt5::Core LOCATION_Release)
  get_filename_component(QT_BINARY_DIR "${QtCore_location_Release}" DIRECTORY)
  add_custom_target(deploy
    COMMAND echo "deploying..."
    COMMAND echo ${QT_BINARY_DIR}/../../bin
    COMMAND ${QT_BINARY_DIR}/../../bin/macdeployqt ${CMAKE_INSTALL_PREFIX}/nootka.app -executable=${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks/libNootkaUpdaterPlugin.dylib ${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/Frameworks/libNootkaSound.dylib
  )
endif(APPLE)


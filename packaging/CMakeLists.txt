##########################################################################################
#			COMMON SETTINGS  ##########################################################
##########################################################################################

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Application to help with learning classical score notation.")
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
if (ENABLE_QTMULTIMEDIA)
  set(DEB_DEPENDS "libqtmultimediakit1")
else (ENABLE_QTMULTIMEDIA)
  if (ENABLE_JACK)
    set(DEB_DEPENDS "libjack0 | libjack-jackd2-0")
    if (ENABLE_PULSEAUDIO)
      set(DEB_DEPENDS "libjack | libjack-jackd2-0, libpulse0")
    endif (ENABLE_PULSEAUDIO)
  else (ENABLE_JACK)
    if (ENABLE_PULSEAUDIO)
      set(DEB_DEPENDS "libpulse0")
    endif (ENABLE_PULSEAUDIO)
  endif (ENABLE_JACK)
endif (ENABLE_QTMULTIMEDIA)

set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.9), libqtcore4 | libqt4-core, libqtgui4 | libqt4-gui, ${DEB_DEPENDS}, libfftw3-3")
#CPACK_DEBIAN_PACKAGE_MAINTAINER #CPACK_PACKAGE_CONTACT
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Application to help with learning classical score notation.
 Mostly it is for guitarists,
 but it can be used for ear training as well.
 .
 Features:
 numerous settings to personalise for user needs and capabilities
 accurate method for detecting sung and played sounds
 different name-callings of the notes
 support for scordatures
 exam mode with different levels to registry progresses
 levels creator with wide range of settings
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
  set(CPACK_GENERATOR  "NSIS")
#   set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/packaging" ${CMAKE_MODULE_PATH}) # Use custom version of NSIS.InstallOptions.ini
else (WIN32)
  if (NOT APPLE)
    set(CPACK_STRIP_FILES "nootka")
    set(CPACK_GENERATOR "RPM;DEB")
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
    execute_process(COMMAND printf "\n\n\nTo build src package call:\n")
    execute_process(COMMAND printf "\tmake src\n\n\n" )

	add_custom_target(src
	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/src/make-src.sh ${Nootka_VERSION_FULL} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}
   )
ENDIF(UNIX AND NOT APPLE)

###########################################################################
# packaging targets (make mac)
###########################################################################
# if(${UNIX})
#     execute_process(COMMAND printf "\n\n\nTo prepare independency of mac boundle:\n")
#     execute_process(COMMAND printf "\tmake mac\n\n\n" )
# 
#   add_custom_target(mac
#   COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/mac/linking.sh ${QT_LIBRARY_DIR} ${QTMULTIMEDIA_LIBRARIES} ${FFTW3_LIBRARIES} ${CMAKE_BINARY_DIR}
#    )
# ENDIF(${UNIX})
#${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}



#### OBSOLETE ##########################
###########################################################################
#	packaging targets (make deb)
###########################################################################
# IF(UNIX AND NOT APPLE)
#     execute_process(COMMAND printf "\n\n\nTo build deb package call:\n")
#     execute_process(COMMAND printf "\tmake deb\n\n\n" )
# 
# 	add_custom_target(deb
# 	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build-deb.sh ${Nootka_VERSION_FULL} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR} "${CMAKE_BINARY_DIR}/debian"
#    )
# 	
# 	add_custom_target(rpm
# 	COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/build-rpm.sh ${Nootka_VERSION_FULL} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR} "${CMAKE_BINARY_DIR}/rpm"
#    )

##########################################################################################
#			COMMON SETTINGS  ##########################################################
##########################################################################################

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Application to help with learning classical score notation.")
set(CPACK_PACKAGE_VENDOR "Nootka")
set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/packaging/nootka-desc")
set(CPACK_RESOURCE_FILE_LICENSE    "${PROJECT_SOURCE_DIR}/LICENSE")

set(CPACK_PACKAGE_VERSION_MAJOR ${Nootka_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${Nootka_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${Nootka_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION ${Nootka_VERSION_FULL})

set(CPACK_PACKAGE_CONTACT "See Look <seelook@gmail.com>")

set(CPACK_PACKAGE_NAME "nootka")


###########################################################################
#	packaging targets (deb)
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
# # ENDIF(UNIX AND NOT APPLE)

##########################################################################################
#		cmake/cpack	DEB  generator 
##########################################################################################
set(CPACK_DEBIAN_PACKAGE_NAME "nootka_${CPACK_PACKAGE_VERSION}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}") #default CPACK_PACKAGE_NAME
# CPACK_DEBIAN_PACKAGE_VERSION #CPACK_PACKAGE_VERSION
# CPACK_DEBIAN_PACKAGE_ARCHITECTURE #dpkg --print-architecture or i386
set(CPACK_DEBIAN_PACKAGE_NAME "nootka" )
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.9), libqt4-core (>= 4.6), libqt4-gui (>= 4.6), libportaudio2")
#CPACK_DEBIAN_PACKAGE_MAINTAINER #CPACK_PACKAGE_CONTACT
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "Application to help with learning classical score notation.
 Mostly it is for guitarists,
 but it can be used for ear training as well.
 .
 Features:
 numerous settings to personalize for user needs and capabilities,
 natural, classical guitar sound output and also midi
 support for different name-callings of the notes,
 scordatures, exams with different levels,
 levels creator with wide range of settings
 .
 http://nootka.sf.net") #CPACK_PACKAGE_DESCRIPTION_SUMMARY
set(CPACK_DEBIAN_PACKAGE_SECTION "Education")
# CPACK_DEBIAN_PACKAGE_PRIORITY #optional
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "http://nootka.sf.net")
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "timidity")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/debian/postrm" )


##########################################################################################
#		cmake/cpack	RPM  generator 
##########################################################################################
# CPACK_RPM_PACKAGE_SUMMARY #CPACK_PACKAGE_DESCRIPTION_SUMMARY
# CPACK_RPM_PACKAGE_NAME # Default   : CPACK_PACKAGE_NAME
# CPACK_RPM_PACKAGE_VERSION # Default   : CPACK_PACKAGE_VERSION
# CPACK_RPM_PACKAGE_ARCHITECTURE # Default   : -
set(CPACK_RPM_PACKAGE_RELEASE "1")
set(CPACK_RPM_PACKAGE_LICENSE "GPLv3")
set(CPACK_RPM_PACKAGE_GROUP "Applications/Education")
set(CPACK_RPM_PACKAGE_VENDOR "Nootka")
set(CPACK_RPM_PACKAGE_URL "http://nootka.sf.net")
# CPACK_RPM_PACKAGE_DESCRIPTION # Default   : CPACK_PACKAGE_DESCRIPTION_FILE if set or "no package description available"
# CPACK_RPM_COMPRESSION_TYPE #  Possible value are: lzma, xz, bzip2 and gzip.
set(CPACK_RPM_PACKAGE_REQUIRES "libc6 >= 2.9, libqt4-core >= 4.6, libqt4-gui >= 4.6, libportaudio2")
set(CPACK_RPM_PACKAGE_SUGGESTS "timidity")
# set(CPACK_RPM_SPEC_INSTALL_POST "/usr/bin/update-desktop-database &> /dev/null || :
# /usr/bin/update-mime-database %{_datadir}/mime &> /dev/null || :")
# CPACK_RPM_SPEC_MORE_DEFINE # May be used to add any %define lines to the generated spec file.
# CPACK_RPM_PACKAGE_DEBUG # cpack -D CPACK_RPM_PACKAGE_DEBUG=1 -G RPM
#          CPACK_RPM_USER_BINARY_SPECFILE
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
#          CPACK_RPM_CHANGELOG_FILE
#             Mandatory : NO
#             Default   : -
#             May be used to embbed a changelog in the spec file.
#             The refered file will be read and directly  put after the %changelog
#             section.



##########################################################################################
#			Resume              ##########################################################
##########################################################################################

if (MINGW)
  set(CPACK_GENERATOR  "NSIS")
#   set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/packaging" ${CMAKE_MODULE_PATH}) # Use custom version of NSIS.InstallOptions.ini
else (MINGW)
  if (NOT APPLE)
    set(CPACK_STRIP_FILES "nootka")
    set(CPACK_GENERATOR "RPM;DEB;TBZ2")
#    set(CPACK_GENERATOR "RPM")
    set(CPACK_DEB "on")
  endif (NOT APPLE)
endif (MINGW)

include (CPack)


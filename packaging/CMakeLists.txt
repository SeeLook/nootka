##########################################################################################
#			COMMON SETTINGS  ##########################################################
##########################################################################################

set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Application to help with learning classical score notation.")
set(CPACK_PACKAGE_VENDOR "Nootka")
# set(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_LICENSE    "${PROJECT_SOURCE_DIR}/LICENSE")

set(CPACK_PACKAGE_VERSION_MAJOR ${Nootka_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${Nootka_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${Nootka_VERSION_PATCH})
## set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_VERSION ${Nootka_VERSION_FULL})

set(CPACK_PACKAGE_CONTACT "seelook@gmail.com")

set(CPACK_PACKAGE_NAME "nootka_${CPACK_PACKAGE_VERSION}")

##########################################################################################
#			DEB                 ##########################################################
##########################################################################################


# CPACK_DEBIAN_PACKAGE_NAME #default CPACK_PACKAGE_NAME
# CPACK_DEBIAN_PACKAGE_VERSION #CPACK_PACKAGE_VERSION
# CPACK_DEBIAN_PACKAGE_ARCHITECTURE #dpkg --print-architecture or i386
set(CPACK_DEBIAN_PACKAGE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}" )
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.9), libqt4-core (>= 4.6), libqt4-gui (>= 4.6), libportaudio2")
#CPACK_DEBIAN_PACKAGE_MAINTAINER #CPACK_PACKAGE_CONTACT
# CPACK_DEBIAN_PACKAGE_DESCRIPTION #CPACK_PACKAGE_DESCRIPTION_SUMMARY
set(CPACK_DEBIAN_PACKAGE_SECTION "Education")
# CPACK_DEBIAN_PACKAGE_PRIORITY #optional
# CPACK_DEBIAN_PACKAGE_HOMEPAGE



##########################################################################################
#			Rsume                 ##########################################################
##########################################################################################

if (MINGW)
  set(CPACK_GENERATOR  "NSIS")
#   set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/packaging" ${CMAKE_MODULE_PATH}) # Use custom version of NSIS.InstallOptions.ini
else (MINGW)
  if (NOT APPLE)
    set(CPACK_GENERATOR "DEB;TBZ2")
    set(CPACK_DEB "on")
  endif (NOT APPLE)
endif (MINGW)

include (CPack)

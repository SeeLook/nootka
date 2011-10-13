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

set(CPACK_PACKAGE_NAME "nootka_${CPACK_PACKAGE_VERSION}")

##########################################################################################
#			DEB                 ##########################################################
##########################################################################################


# CPACK_DEBIAN_PACKAGE_NAME #default CPACK_PACKAGE_NAME
# CPACK_DEBIAN_PACKAGE_VERSION #CPACK_PACKAGE_VERSION
# CPACK_DEBIAN_PACKAGE_ARCHITECTURE #dpkg --print-architecture or i386
set(CPACK_DEBIAN_PACKAGE_NAME "nootka" )
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6 (>= 2.9), libqt4-core (>= 4.6), libqt4-gui (>= 4.6), libportaudio2")
#CPACK_DEBIAN_PACKAGE_MAINTAINER #CPACK_PACKAGE_CONTACT
# CPACK_DEBIAN_PACKAGE_DESCRIPTION #CPACK_PACKAGE_DESCRIPTION_SUMMARY
set(CPACK_DEBIAN_PACKAGE_SECTION "Education")
# CPACK_DEBIAN_PACKAGE_PRIORITY #optional
# CPACK_DEBIAN_PACKAGE_HOMEPAGE
set(CPACK_DEBIAN_PACKAGE_RECOMMENDS "timidity")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/packaging/debian/postinst;${CMAKE_CURRENT_SOURCE_DIR}/packaging/debian/prerm;" )


##########################################################################################
#			Resume              ##########################################################
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





# mime-types associations under linux
# was worked fine
# if(UNIX AND NOT APPLE)
#     INSTALL(FILES mime/nootka.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications")
#     INSTALL(FILES picts/nootka.svg DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pixmaps")
#     INSTALL( CODE"
#       execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/mime )
#       execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/mime/install-mime.sh ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}/mime ${CMAKE_CURRENT_SOURCE_DIR}/mime)" )
# 
#     INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/mime DESTINATION "${CMAKE_INSTALL_PREFIX}/share/nootka")
#     find_program(XDG-MIME_EXECUTABLE xdg-mime)
#     find_program(XDG-ICON_EXECUTABLE xdg-icon-resource)
#     find_program(XDG-MENU_EXECUTABLE xdg-desktop-menu)
#     INSTALL( CODE"
#       execute_process(COMMAND echo 'Installing mime-types' )
#       execute_process(COMMAND ${XDG-MIME_EXECUTABLE} install --novendor ${CMAKE_CURRENT_BINARY_DIR}/mime/nootka.xml)
#       execute_process(COMMAND ${XDG-MENU_EXECUTABLE} install --novendor ${CMAKE_CURRENT_SOURCE_DIR}/mime/nootka.desktop)
#       execute_process(COMMAND ${XDG-MIME_EXECUTABLE} default nootka.desktop application/x-nootka-noo)
#       execute_process(COMMAND ${XDG-MIME_EXECUTABLE} default nootka.desktop application/x-nootka-nel)
#       execute_process(COMMAND ${XDG-ICON_EXECUTABLE} install --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/pixmaps/levelCreator.png application-x-nootka-nel)
#       execute_process(COMMAND ${XDG-ICON_EXECUTABLE} install --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/pixmaps/nootka-exam.png application-x-nootka-noo)"
#     )
# 
#   add_custom_target(uninstall-mime
#      COMMAND echo "uninstalling mime-types"
#      COMMAND ${XDG-MIME_EXECUTABLE} uninstall --novendor ${CMAKE_INSTALL_PREFIX}/share/nootka/mime/nootka.xml
#      COMMAND ${XDG-MENU_EXECUTABLE} uninstall --novendor ${CMAKE_INSTALL_PREFIX}/share/applications/nootka.desktop
#      COMMAND ${XDG-ICON_EXECUTABLE} uninstall --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/pixmaps/levelCreator.png application-x-nootka-nel
#      COMMAND ${XDG-ICON_EXECUTABLE} uninstall --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/pixmaps/nootka-exam.png application-x-nootka-noo

# 
# endif(UNIX AND NOT APPLE)


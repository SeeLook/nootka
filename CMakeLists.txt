project(nootka)
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_BUILD_TYPE Release)
find_package(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )
add_definitions(${QT_DEFINITIONS})

set(CMAKE_COLOR_MAKEFILE ON)

include_directories(${QT_INCLUDES})

add_subdirectory(src)


#INSTALLATION
if(UNIX AND NOT APPLE)
#     INSTALL(TARGETS nootka DESTINATION bin)
    INSTALL(FILES LICENSE DESTINATION share/nootka)
    INSTALL(DIRECTORY picts DESTINATION share/nootka)
    INSTALL(DIRECTORY fonts DESTINATION share/nootka)

    find_program(XDG-MIME_EXECUTABLE xdg-mime)
    find_program(XDG-DESKTOP-MENU_EXECUTABLE xdg-desktop-menu)
    execute_process(COMMAND ${XDG-MIME_EXECUTABLE} install --novendor nootka.xml)
    execute_process(COMMAND ${XDG-DESKTOP-MENU_EXECUTABLE} install --novendor nootka.desktop)
    execute_process(COMMAND ${XDG-MIME_EXECUTABLE} default nootka.desktop "application/x-nootka-item")

    # uninstall target
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif(UNIX AND NOT APPLE)

#BUILD PACKAGES

INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Nootka")
SET(CPACK_PACKAGE_VENDOR "SeeLook")
SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
SET(CPACK_PACKAGE_VERSION_MAJOR "0")
SET(CPACK_PACKAGE_VERSION_MINOR "7")
SET(CPACK_PACKAGE_VERSION_PATCH "0")
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSI that does not handle full unix paths properly. Make
  # sure there is at least one set of four (4) backlasshes.
  SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/picts\\\\logo.bmp")
#   SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} nootka")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\www.nootka.sf.net")
#   SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.my-personal-home-page.com")
  SET(CPACK_NSIS_CONTACT "seelook@gmail.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "bin/nootka")
  SET(CPACK_SOURCE_STRIP_FILES "")
  SET(CPACK_GENERATOR "DEB")
  SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "SeeLook") #required 
ENDIF(WIN32 AND NOT UNIX)
SET(CPACK_PACKAGE_EXECUTABLES "nootka" "nootka")
INCLUDE(CPack)

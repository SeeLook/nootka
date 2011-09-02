project(nootka)
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_BUILD_TYPE Release)
find_package(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )
add_definitions(${QT_DEFINITIONS})

set(CMAKE_COLOR_MAKEFILE ON)

include_directories(${QT_INCLUDES})

###########################################################################
########   Looking for Portaudio
###########################################################################
IF(PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)
  set(PORTAUDIO_FOUND TRUE)
ELSE(PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)
  find_path(PORTAUDIO_INCLUDE_DIR
    NAMES
      portaudio.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
      ${CMAKE_CURRENT_SOURCE_DIR}
      ${CMAKE_CURRENT_BINARY_DIR}
  )
  IF(WIN32)
            find_library(PORTAUDIO_LIBRARY
	    NAMES
	      libportaudio-2.dll
	    PATHS
	      ${CMAKE_CURRENT_SOURCE_DIR}
	      ${CMAKE_CURRENT_BINARY_DIR}
	  )
  ELSE(WIN32)
	  find_library(PORTAUDIO_LIBRARY
	    NAMES
	      portaudio
	    PATHS
	      /usr/lib
	      /usr/local/lib
	      /opt/local/lib
	      /sw/lib
	      ${CMAKE_CURRENT_BINARY_DIR}
	      ${CMAKE_CURRENT_SOURCE_DIR}
  )
  ENDIF(WIN32)

  set(PORTAUDIO_INCLUDE_DIRS
    ${PORTAUDIO_INCLUDE_DIR}
  )
  set(PORTAUDIO_LIBRARIES
    ${PORTAUDIO_LIBRARY}
)

  IF(PORTAUDIO_INCLUDE_DIRS AND PORTAUDIO_LIBRARIES)
     set(PORTAUDIO_FOUND TRUE)
  ENDIF(PORTAUDIO_INCLUDE_DIRS AND PORTAUDIO_LIBRARIES)

  IF(PORTAUDIO_FOUND)
      message(STATUS "Portaudio was Found in:\n${PORTAUDIO_LIBRARIES}\n${PORTAUDIO_INCLUDE_DIRS}")
  ELSE(PORTAUDIO_FOUND)
    #if (Portaudio_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find Portaudio\nTry call cmake with args:\n-DPORTAUDIO_LIBRARIES=/path_to/libportaudio.la(dll) -DPORTAUDIO_INCLUDE_DIRS=/path_to_dir_with/portaudio.h\nor put those files in build dir or source dir")
    #endif (Portaudio_FIND_REQUIRED)
  ENDIF(PORTAUDIO_FOUND)
  mark_as_advanced(PORTAUDIO_INCLUDE_DIRS PORTAUDIO_LIBRARIES)
ENDIF(PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)


add_subdirectory(src)


###########################################################################
#INSTALLATION
###########################################################################
IF(WIN32)
    set(INS_PREF "")
ELSE(WIN32)
    set(INS_PREF "/share/nootka")
ENDIF(WIN32)

INSTALL(FILES LICENSE DESTINATION ${CMAKE_INSTALL_PREFIX}${INS_PREF})
INSTALL(DIRECTORY picts DESTINATION ${CMAKE_INSTALL_PREFIX}${INS_PREF})
INSTALL(DIRECTORY fonts DESTINATION ${CMAKE_INSTALL_PREFIX}${INS_PREF})
FILE(GLOB langs "${CMAKE_CURRENT_SOURCE_DIR}/lang/*.qm")
INSTALL(FILES ${langs} DESTINATION "${CMAKE_INSTALL_PREFIX}${INS_PREF}/lang")
FILE(GLOB snds "${CMAKE_CURRENT_SOURCE_DIR}/sounds/*.wav")
INSTALL(FILES ${snds} DESTINATION "${CMAKE_INSTALL_PREFIX}${INS_PREF}/sounds")

# mime-types associations under linux
if(UNIX AND NOT APPLE)
    INSTALL(FILES mime/nootka.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications")
    INSTALL(FILES picts/nootka.svg DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pixmaps")
    INSTALL( CODE"
      execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/mime )
      execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/mime/install-mime.sh ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}/mime ${CMAKE_CURRENT_SOURCE_DIR}/mime)" )

    INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/mime DESTINATION "${CMAKE_INSTALL_PREFIX}/share/nootka")
    find_program(XDG-MIME_EXECUTABLE xdg-mime)
    find_program(XDG-ICON_EXECUTABLE xdg-icon-resource)
    find_program(XDG-MENU_EXECUTABLE xdg-desktop-menu)
    INSTALL( CODE"
      execute_process(COMMAND echo 'Installing mime-types' )
      execute_process(COMMAND ${XDG-MIME_EXECUTABLE} install --novendor ${CMAKE_CURRENT_BINARY_DIR}/mime/x-nootka.xml)
      execute_process(COMMAND ${XDG-MENU_EXECUTABLE} install --novendor ${CMAKE_CURRENT_SOURCE_DIR}/mime/nootka.desktop)
      execute_process(COMMAND ${XDG-MIME_EXECUTABLE} default nootka.desktop application/x-nootka-noo)
      execute_process(COMMAND ${XDG-MIME_EXECUTABLE} default nootka.desktop application/x-nootka-nel)
      execute_process(COMMAND ${XDG-ICON_EXECUTABLE} install --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/nootka/picts/levelCreator.png application-x-nootka-nel)
      execute_process(COMMAND ${XDG-ICON_EXECUTABLE} install --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/nootka/picts/nootka-exam.png application-x-nootka-noo)"
    )
endif(UNIX AND NOT APPLE)

###########################################################################
# uninstall target
###########################################################################
configure_file(
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
     "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

IF(UNIX AND NOT APPLE)
  add_custom_target(uninstall
     COMMAND echo "uninstalling mime-types"
     COMMAND ${XDG-MIME_EXECUTABLE} uninstall --novendor ${CMAKE_INSTALL_PREFIX}/share/nootka/mime/x-nootka.xml
     COMMAND ${XDG-MENU_EXECUTABLE} uninstall --novendor ${CMAKE_INSTALL_PREFIX}/share/applications/nootka.desktop
     COMMAND ${XDG-ICON_EXECUTABLE} uninstall --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/nootka/picts/levelCreator.png application-x-nootka-nel
     COMMAND ${XDG-ICON_EXECUTABLE} uninstall --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/nootka/picts/nootka-exam.png application-x-nootka-noo
   COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)
ELSE(UNIX AND NOT APPLE)
    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
  )
ENDIF(UNIX AND NOT APPLE)

###########################################################################
#	cpack rules
###########################################################################

IF(MINGW)
		include (InstallRequiredSystemLibraries)

		SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Nootka - to play scores")
		SET(CPACK_PACKAGE_VENDOR "Nootka")
		SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/LICENSE")
		SET(CPACK_RESOURCE_FILE_LICENSE    "${PROJECT_SOURCE_DIR}/LICENSE")

		SET(CPACK_PACKAGE_VERSION_MAJOR "0")
		SET(CPACK_PACKAGE_VERSION_MINOR "7")
		SET(CPACK_PACKAGE_VERSION_PATCH "5")
		SET(CPACK_PACKAGE_INSTALL_DIRECTORY "Nootka ${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")

		SET(CPACK_NSIS_COMPRESSOR "/FINAL /SOLID lzma")

		SET(CPACK_PACKAGE_INSTALL_DIRECTORY "nootka")
		# There is a bug in NSI that does not handle full unix paths properly. Make
		# sure there is at least one set of four (4) backlasshes.
		SET(CPACK_PACKAGE_ICON "${PROJECT_SOURCE_DIR}/picts\\\\logo.bmp")
		SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\nootka.exe,0")
		SET(CPACK_NSIS_DISPLAY_NAME ${CPACK_PACKAGE_DESCRIPTION_SUMMARY})
		SET(CPACK_NSIS_HELP_LINK "http://www.nootka.sf.net/")
		SET(CPACK_NSIS_URL_INFO_ABOUT "http://www.nootka.sf.net/")
		SET(CPACK_NSIS_CONTACT "seelook@gmail.com")
		SET(CPACK_NSIS_MODIFY_PATH OFF)
		SET(CPACK_STRIP_FILES "nootka.exe")

		# File types association:
		SET(CPACK_NSIS_DEFINES "!include ${PROJECT_SOURCE_DIR}/packaging\\\\FileAssociation.nsh")

		SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
			Push \\\"ATENDATA\\\"
			Push \\\"$INSTDIR\\\\share\\\\aten\\\"
			Call WriteEnvStr
		")

		SET(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
			\\\${registerExtension} \\\"Nootka exam level file\\\" \\\".nel\\\" \\\"\\\$INSTDIR\\\\nootka.exe\\\"
			\\\${registerExtension} \\\"Nootka exam results file\\\" \\\".noo\\\" \\\"\\\$INSTDIR\\\\nootka.exe\\\"
		")
		SET(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "
			\\\${unregisterExtension} \\\".nel\\\" \\\"Nootka exam level file\\\"
			\\\${unregisterExtension} \\\".noo\\\" \\\"Nootka exam results file\\\"
		")
		
		include (CPack)
  ENDIF(MINGW)


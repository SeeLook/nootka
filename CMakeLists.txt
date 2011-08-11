project(nootka)
cmake_minimum_required(VERSION 2.8)
SET(CMAKE_BUILD_TYPE Release)
find_package(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )
add_definitions(${QT_DEFINITIONS})

set(CMAKE_COLOR_MAKEFILE ON)

include_directories(${QT_INCLUDES})


########   Looking for Portaudio
if (PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)
  # in cache already
  set(PORTAUDIO_FOUND TRUE)
else (PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)
  find_path(PORTAUDIO_INCLUDE_DIR
    NAMES
      portaudio.h
    PATHS
      /usr/include
      /usr/local/include
      /opt/local/include
      /sw/include
      ${CMAKE_CURRENT_BINARY_DIR}
  )

  find_library(PORTAUDIO_LIBRARY
    NAMES
      portaudio
    PATHS
      /usr/lib
      /usr/local/lib
      /opt/local/lib
      /sw/lib
      ${CMAKE_CURRENT_BINARY_DIR}
  )

  set(PORTAUDIO_INCLUDE_DIRS
    ${PORTAUDIO_INCLUDE_DIR}
  )
  set(PORTAUDIO_LIBRARIES
    ${PORTAUDIO_LIBRARY}
)

  if (PORTAUDIO_INCLUDE_DIRS AND PORTAUDIO_LIBRARIES)
     set(PORTAUDIO_FOUND TRUE)
  endif (PORTAUDIO_INCLUDE_DIRS AND PORTAUDIO_LIBRARIES)

  if (PORTAUDIO_FOUND)
    if (NOT Portaudio_FIND_QUIETLY)
      message(STATUS "Found Portaudio: ${PORTAUDIO_LIBRARIES}")
    endif (NOT Portaudio_FIND_QUIETLY)
  else (PORTAUDIO_FOUND)
    if (Portaudio_FIND_REQUIRED)
      message(FATAL_ERROR "Could not find Portaudio")
    endif (Portaudio_FIND_REQUIRED)
  endif (PORTAUDIO_FOUND)

  # show the PORTAUDIO_INCLUDE_DIRS and PORTAUDIO_LIBRARIES variables only in the advanced view
  mark_as_advanced(PORTAUDIO_INCLUDE_DIRS PORTAUDIO_LIBRARIES)

endif (PORTAUDIO_LIBRARIES AND PORTAUDIO_INCLUDE_DIRS)


add_subdirectory(src)


#INSTALLATION
if(UNIX AND NOT APPLE)
    INSTALL(FILES LICENSE DESTINATION "${CMAKE_INSTALL_PREFIX}/share/nootka")
    INSTALL(DIRECTORY picts DESTINATION "${CMAKE_INSTALL_PREFIX}/share/nootka")
    INSTALL(DIRECTORY fonts DESTINATION "${CMAKE_INSTALL_PREFIX}/share/nootka")

    INSTALL(FILES mime/nootka.desktop DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications")
    INSTALL(FILES picts/nootka.svg DESTINATION "${CMAKE_INSTALL_PREFIX}/share/pixmaps")

    execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/mime )
    execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/mime/install-mime.sh ${CMAKE_INSTALL_PREFIX} ${CMAKE_CURRENT_BINARY_DIR}/mime ${CMAKE_CURRENT_SOURCE_DIR}/mime)

    INSTALL(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/mime DESTINATION "${CMAKE_INSTALL_PREFIX}/share/nootka")
    find_program(XDG-MIME_EXECUTABLE xdg-mime)
    find_program(XDG-ICON_EXECUTABLE xdg-icon-resource)
    find_program(XDG-MENU_EXECUTABLE xdg-desktop-menu)
    INSTALL( CODE"
      execute_process(COMMAND echo 'Installing mime-types' )
      execute_process(COMMAND ${XDG-MIME_EXECUTABLE} install --novendor ${CMAKE_CURRENT_BINARY_DIR}/mime/x-nootka-noo.xml)
      execute_process(COMMAND ${XDG-MIME_EXECUTABLE} install --novendor ${CMAKE_CURRENT_BINARY_DIR}/mime/x-nootka-nel.xml)
      execute_process(COMMAND ${XDG-MENU_EXECUTABLE} install --novendor ${CMAKE_CURRENT_SOURCE_DIR}/mime/nootka.desktop)
      execute_process(COMMAND ${XDG-MIME_EXECUTABLE} default nootka.desktop application/x-nootka-noo)
      execute_process(COMMAND ${XDG-MIME_EXECUTABLE} default nootka.desktop application/x-nootka-nel)
      execute_process(COMMAND ${XDG-ICON_EXECUTABLE} install --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/nootka/picts/levelCreator.png application-x-nootka-nel)
      execute_process(COMMAND ${XDG-ICON_EXECUTABLE} install --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/nootka/picts/nootka-exam.png application-x-nootka-noo)"
    )
#       execute_process(COMMAND sh install-mime.sh ${CMAKE_INSTALL_PREFIX} )

  # uninstall target
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
	COMMAND echo "uninstalling mime-types"
	COMMAND ${XDG-MIME_EXECUTABLE} uninstall --novendor ${CMAKE_INSTALL_PREFIX}/share/nootka/mime/x-nootka-noo.xml
	COMMAND ${XDG-MIME_EXECUTABLE} uninstall --novendor ${CMAKE_INSTALL_PREFIX}/share/nootka/mime/x-nootka-nel.xml
	COMMAND ${XDG-MENU_EXECUTABLE} uninstall --novendor ${CMAKE_INSTALL_PREFIX}/share/applications/nootka.desktop
	COMMAND ${XDG-ICON_EXECUTABLE} uninstall --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/nootka/picts/levelCreator.png application-x-nootka-nel
	COMMAND ${XDG-ICON_EXECUTABLE} uninstall --context mimetypes --size 128 ${CMAKE_INSTALL_PREFIX}/share/nootka/picts/nootka-exam.png application-x-nootka-noo
	COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )
endif(UNIX AND NOT APPLE)


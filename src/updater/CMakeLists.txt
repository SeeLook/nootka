########################################################################
# CMakeList.txt nootka-updater
########################################################################

option(ENABLE_UPDATER "Enable online checking for new Nootka versions" ON)

if(ENABLE_UPDATER)
  message(STATUS "check for updates\t\t TRUE")
  find_package(Qt5Network)
    SET( UPDATER_SRCS
      main.cpp
      tupdatechecker.cpp
      updatefunctions.cpp
      tupdateruleswdg.cpp
      tupdatesummary.cpp
		)

    if(UNIX AND NOT APPLE)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-z,relro")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,relro")
    endif(UNIX AND NOT APPLE)


    if(MINGW)
    # dress up win executable in nice nootka icon ONLY FOR MINGW !!!!!!!!!!!!!
    #     ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/nootico.o
    #                        COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/../picts/nootka-icon.rc
    #                             -o ${CMAKE_CURRENT_BINARY_DIR}/nootico.o )
    #     SET(UPDATER_SRCS ${UPDATER_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/nootico.o)
        ADD_EXECUTABLE( nootka-updater 
          WIN32 
          ${UPDATER_SRCS} 
          ${UPDATER_MOCS} 
        )
      TARGET_LINK_LIBRARIES(nootka-updater
            ${QT_QTCORE_LIBRARY}
            ${QT_QTGUI_LIBRARY}
            ${QT_QTNETWORK_LIBRARY}
      )
    endif(MINGW)

    if(UNIX AND NOT APPLE) #Linux
			ADD_EXECUTABLE( nootka-updater ${UPDATER_SRCS} )
			qt5_use_modules( nootka-updater Widgets Network )
#       TARGET_LINK_LIBRARIES( nootka-updater Qt5::Widgets Qt5::Network )
    endif(UNIX AND NOT APPLE)

    if(APPLE)
    #     set (MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/packaging/mac/info.plist")
    #    set (ExeUpName nootka-updater MACOSX_BUNDLE)
    #    set (MACOSX_BUNDLE_INFO_STRING Nootka release for Mac OSX)
    #    set (MACOSX_BUNDLE_ICON_FILE      "nootka.icns")
    #    set (MACOSX_BUNDLE_GUI_IDENTIFIER net.sourceforge.nootka)
    #    set (MACOSX_BUNDLE_LONG_VERSION_STRING ${Nootka_VERSION_FULL})
    #    set (MACOSX_BUNDLE_BUNDLE_NAME Nootka)
    #    set (MACOSX_BUNDLE_SHORT_VERSION_STRING ${Nootka_VERSION_FULL})
    #    set (MACOSX_BUNDLE_BUNDLE_VERSION ${Nootka_VERSION_FULL})
    #    set (MACOSX_BUNDLE_COPYRIGHT nootka.sf.net)
        ADD_EXECUTABLE( nootka-updater # ${ExeUpName} # nootka-updater # ${ExeName}
            ${UPDATER_SRCS}
            ${UPDATER_MOCS}
        )
      TARGET_LINK_LIBRARIES(nootka-updater
            ${QT_QTCORE_LIBRARY}
            ${QT_QTGUI_LIBRARY}
            ${QT_QTNETWORK_LIBRARY}
      )
    endif(APPLE)




    if(UNIX AND NOT APPLE) 	# Linux path for Nootka binary
      install(TARGETS nootka-updater DESTINATION bin)
    else(UNIX AND NOT APPLE)
      if(WIN32) 			# Windows
        install(TARGETS nootka-updater DESTINATION .)
      else(WIN32) 			# MacOs
    #     install(TARGETS nootka-updater BUNDLE DESTINATION ${CMAKE_INSTALL_PREFIX})
        install(TARGETS nootka-updater DESTINATION "nootka.app/Contents/MacOS/")
      endif(WIN32)
    endif(UNIX AND NOT APPLE)
    
else(ENABLE_UPDATER)
    message(STATUS "check for updates\t\t FALSE")
endif(ENABLE_UPDATER)

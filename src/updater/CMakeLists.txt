
SET( UPDATER_SRCS
  main.cpp
  tupdatechecker.cpp
  )
  
  
SET( UPDATER_MOC_SRCS
  tupdatechecker.h
  )


QT4_WRAP_CPP( UPDATER_MOCS ${UPDATER_MOC_SRCS} )

# if(LINUX)
#     set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro")
#     set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-z,relro")
#     set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,relro")
# endif(LINUX)


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
    ADD_EXECUTABLE( nootka-updater
        ${UPDATER_SRCS}
        ${UPDATER_MOCS}
    #        ${NOOTKA_RCC_SRCS}
    )
	TARGET_LINK_LIBRARIES(nootka-updater
        ${QT_QTCORE_LIBRARY}
        ${QT_QTGUI_LIBRARY}
        ${QT_QTNETWORK_LIBRARY}
	)
endif(UNIX AND NOT APPLE)

if(APPLE)
#     set (MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/packaging/mac/info.plist")
#    set (ExeName nootka MACOSX_BUNDLE)
#    set (MACOSX_BUNDLE_INFO_STRING Nootka release for Mac OSX)
#    set (MACOSX_BUNDLE_ICON_FILE      "nootka.icns")
#    set (MACOSX_BUNDLE_GUI_IDENTIFIER net.sourceforge.nootka)
#    set (MACOSX_BUNDLE_LONG_VERSION_STRING ${Nootka_VERSION_FULL})
#    set (MACOSX_BUNDLE_BUNDLE_NAME Nootka)
#    set (MACOSX_BUNDLE_SHORT_VERSION_STRING ${Nootka_VERSION_FULL})
#    set (MACOSX_BUNDLE_BUNDLE_VERSION ${Nootka_VERSION_FULL})
#    set (MACOSX_BUNDLE_COPYRIGHT nootka.sf.net)
    ADD_EXECUTABLE(  nootka-updater # ${ExeName}
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


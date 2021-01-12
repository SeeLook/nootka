########################################################################
# CMakeList.txt src
########################################################################

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)


find_package(Qt5Widgets REQUIRED)
if (Qt5Widgets_FOUND)
    if (Qt5Widgets_VERSION VERSION_LESS 5.9.0)
        message(FATAL_ERROR "Minimum supported Qt version is 5.9.0")
    else()
        if (Qt5Widgets_VERSION VERSION_GREATER 5.14 AND CMAKE_BUILD_TYPE MATCHES Release)
          # TODO
          # Nootka is not compatible with Qt 6 - needs porting,
          # to keep less demanding of Qt 5 version (5.9 so far enough).
          # Hide warnings then of Qt 5.15 for clean release build.
          #
          message("===== For RELEASE build suppress Qt 5.15 warnings about deprecated declarations ====")
          set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
        endif()
    endif(Qt5Widgets_VERSION VERSION_LESS 5.9.0)
else()
    message(SEND_ERROR "The Qt5Widgets library could not be found!")
endif(Qt5Widgets_FOUND)


find_package(Qt5Qml REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5QuickControls2 REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)


if(UNIX AND NOT APPLE)
  if (PACKAGE_ARCHITECTURE MATCHES "arm" OR PACKAGE_ARCHITECTURE MATCHES "aarch64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsigned-char")
    message("Set additional flag -fsigned-char for ${PACKAGE_ARCHITECTURE} arch")
  endif(PACKAGE_ARCHITECTURE MATCHES "arm" OR PACKAGE_ARCHITECTURE MATCHES "aarch64")
endif(UNIX AND NOT APPLE)


if(UNIX AND NOT APPLE)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-z,relro")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -Wl,-z,relro")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,-z,relro")
# rpath rules
  option(OBS_SUSE_QUIRKS "Must be ON for Suse machines in Open Build Services" OFF)

  if (NOT WITH_STATIC_LIB) # no rpaths when Nootka libraries are linked statically
    set(NOOTKA_LIB_PATH "${CMAKE_INSTALL_PREFIX}/lib/nootka")
    set(CMAKE_SKIP_BUILD_RPATH  FALSE) # use, i.e. don't skip the full RPATH for the build tree
    if(OBS_SUSE_QUIRKS)
      set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
      message("Build for Suse OBS")
    else(OBS_SUSE_QUIRKS)
      set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE) # when building, don't use the install RPATH already (but later on when installing)
    endif(OBS_SUSE_QUIRKS)
    set(CMAKE_INSTALL_RPATH "${NOOTKA_LIB_PATH}")
    # add the automatically determined parts of the RPATH
    # which point to directories outside the build tree to the install RPATH
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)

    # the RPATH to be used when installing, but only if it's not a system directory
    list(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${NOOTKA_LIB_PATH}" isSystemDir)
    if("${isSystemDir}" STREQUAL "-1")
      set(CMAKE_INSTALL_RPATH "${NOOTKA_LIB_PATH}")
    endif("${isSystemDir}" STREQUAL "-1")
  endif (NOT WITH_STATIC_LIB)
endif(UNIX AND NOT APPLE)

if (APPLE) # Mac RPath
  set(CMAKE_MACOSX_RPATH 1)
  set(CMAKE_INSTALL_RPATH "@loader_path/../Frameworks")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif (APPLE)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  add_definitions(-DQT_QML_DEBUG)
endif ()


option (WITH_STATIC_LIB "Link Nootka libraries statically - used for AppImage" OFF)


add_subdirectory( libs/core )     # libNootkaCore
add_subdirectory( libs/sound )    # libNootkaSound

include_directories( libs/core libs/sound ) # libs/main


# nootka executable
set(NOOTKA_SRC
  main.cpp
  main/tnameitem.cpp

  main/texamexecutor.cpp
  main/texecutorsupply.cpp
  main/tglobalexamstore.cpp
  main/texercises.cpp
  main/tequalrand.cpp
  main/texammelody.cpp
  main/trandmelody.cpp
  main/texamview.cpp
  main/tpenalty.cpp
  main/ttiphandler.cpp
  main/tmainscoreobject.cpp
  main/texamsummary.cpp
  main/tstartexamitem.cpp
  main/tnootkacertificate.cpp
  main/tpiechartitem.cpp
  main/tnotetoplay.cpp

  dialogs/tdialogloaderobject.cpp
  dialogs/tlevelcreatoritem.cpp
  dialogs/tlevelpreviewitem.cpp
  dialogs/tlevelsdefs.cpp
  dialogs/tlevelselector.cpp
  dialogs/tlevelheaderwdg.cpp
  dialogs/tmelodylistview.cpp
  dialogs/tmelodywrapper.cpp
  dialogs/ttunerdialogitem.cpp
  dialogs/trtmselectoritem.cpp
  dialogs/tmelgenitem.cpp

  help/thelpdialogbase.cpp
  help/texamhelp.cpp
  help/tmainhelp.cpp

  charts/tgroupedqaunit.cpp
  charts/sorting.cpp
  charts/tchart.cpp
  charts/tmainchart.cpp
  charts/tlinearchart.cpp
  charts/tchartitem.cpp
  charts/tcharttipitem.cpp
  charts/tbarchart.cpp
  charts/tlinchartdelegate.cpp
  charts/tbarchartdelegate.cpp

  nootini/taudioanalyzeitem.cpp
)


option(ENABLE_UPDATER "Enable online checking for new Nootka versions" ON)


if(ENABLE_UPDATER)
  message(STATUS "check for updates\t\t TRUE")
  find_package(Qt5Network)
  list(APPEND NOOTKA_SRC
    updater/tupdatechecker.cpp
    updater/updatefunctions.cpp
    updater/tupdateruleswdg.cpp
    updater/tupdatesummary.cpp
  )

else(ENABLE_UPDATER)
    message(STATUS "check for updates\t\t FALSE")
endif(ENABLE_UPDATER)


if(MINGW)
    add_custom_command( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/nootico.o
                       COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/../picts/nootka-icon.rc
                            -o ${CMAKE_CURRENT_BINARY_DIR}/nootico.o )
    set(NOOTKA_SRCS ${NOOTKA_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/nootico.o)
    set(NOOTKA_EXE_ICON nootico.o)
endif(MINGW)


qt5_add_resources(NOOTKA_SRC nootka.qrc)


add_executable(nootka WIN32 ${NOOTKA_SRC} ${NOOTKA_EXE_ICON})
target_link_libraries(nootka
    NootkaCore
    NootkaSound
    Qt5::Core
    Qt5::Widgets
    Qt5::Qml
    Qt5::Quick
    Qt5::QuickControls2
    Qt5::PrintSupport
)


if(UNIX AND NOT APPLE)   # Linux path for Nootka binary
  install(TARGETS nootka DESTINATION bin)
else(UNIX AND NOT APPLE)
  if(WIN32)       # Windows
    install(TARGETS nootka DESTINATION .)
  else(WIN32)       # MacOs
    install(TARGETS nootka DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/MacOs")
    install( CODE "
    execute_process(COMMAND echo copying info.plist )
    execute_process(COMMAND sh ${CMAKE_SOURCE_DIR}/packaging/mac/grabInfo.sh ${Nootka_VERSION_FULL} ${CMAKE_SOURCE_DIR} ${CMAKE_INSTALL_PREFIX} )"
    )
  endif(WIN32)
endif(UNIX AND NOT APPLE)

# add_subdirectory( plugins )


# get_directory_property( DirDefs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} COMPILE_DEFINITIONS )
# message(STATUS "${CMAKE_CURRENT_SOURCE_DIR} : ${DirDefs}" )


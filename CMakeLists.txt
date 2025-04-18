########################################################################
# MAIN CMAKE FILE FOR Nootka
########################################################################

cmake_minimum_required(VERSION 3.16.0)

if (POLICY CMP0071)
  cmake_policy(SET CMP0071 NEW)
endif()

project(nootka)

set(Nootka_VERSION_MAJOR  "2")
set(Nootka_VERSION_MINOR  "1")
set(Nootka_VERSION_PATCH  "7-devel")

set(QT_DEFAULT_MAJOR_VERSION 6)

set(Nootka_VERSION       "${Nootka_VERSION_MAJOR}.${Nootka_VERSION_MINOR}")
set(Nootka_VERSION_FULL  "${Nootka_VERSION}.${Nootka_VERSION_PATCH}")

message(STATUS "\n${PROJECT_NAME}-${Nootka_VERSION_FULL}\n")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/src)

if(UNIX AND NOT APPLE) # config with version number is generated once on developer platform
    configure_file(${CMAKE_SOURCE_DIR}/src/nootkaconfig.h.in ${CMAKE_SOURCE_DIR}/src/core/nootkaconfig.h)
endif (UNIX AND NOT APPLE)

if(APPLE)
    install(CODE "
        execute_process(
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_PREFIX}/nootka.app/Contents)
        "
    )
endif ()

###########################################################################
#INSTALLATION
###########################################################################
if(WIN32)
    set(PACKAGE_ARCHITECTURE x86)
    option(COPY_GCC_LIBS "Copy compiler specific libraries during install target" ON)
endif(WIN32)

if(APPLE) # TODO: Apple Silicon
  set(PACKAGE_ARCHITECTURE x86_64)
endif(APPLE)

if(NOT PACKAGE_ARCHITECTURE)
    find_program(DPKG_CMD dpkg)
    if (DPKG_CMD)
        execute_process(COMMAND
            "${DPKG_CMD}" --print-architecture
            OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
            OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    else()
        find_program(RPM_CMD rpm)
        if (RPM_CMD)
            execute_process(COMMAND
                "${RPM_CMD}" --eval %_target_cpu
                OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
                OUTPUT_STRIP_TRAILING_WHITESPACE 
            )
            set(RPM_CMD_FOUND TRUE)
        else()
            execute_process(COMMAND
                uname -m
                OUTPUT_VARIABLE PACKAGE_ARCHITECTURE
                OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        endif()
    endif()
endif()
message(STATUS "Architecture is ${PACKAGE_ARCHITECTURE}")

if (NOT DPKG_CMD)
    unset(DPKG_CMD)
endif ()

add_subdirectory(src)

if(WIN32)
    set(INS_PREF ".")
else()
    if (APPLE)
        set(INS_PREF "nootka.app/Contents/Resources")
    else()
        set(INS_PREF "share/nootka")
    endif()
endif()

if(APPLE)
    install(FILES "${CMAKE_SOURCE_DIR}/picts/nootka.icns" DESTINATION "${INS_PREF}") # icns for mac
    install(FILES "${CMAKE_SOURCE_DIR}/picts/nootka-levels.icns" DESTINATION "${INS_PREF}")
    install(FILES "${CMAKE_SOURCE_DIR}/picts/nootka-exam.icns" DESTINATION "${INS_PREF}")
    install(FILES "${CMAKE_SOURCE_DIR}/packaging/mac/qt.conf" DESTINATION "${INS_PREF}")
endif()

install(FILES changes DESTINATION "${INS_PREF}")
file(GLOB ICONS_PNG "${CMAKE_CURRENT_SOURCE_DIR}/picts/*.png") #png-s for all
install(FILES ${ICONS_PNG} DESTINATION "${INS_PREF}/picts")
install(DIRECTORY picts/flags DESTINATION "${INS_PREF}/picts")
install(DIRECTORY picts/pane DESTINATION "${INS_PREF}/picts")
install(DIRECTORY fonts DESTINATION "${INS_PREF}")

install(DIRECTORY "levels/" DESTINATION "${INS_PREF}/levels")

if(APPLE OR WIN32)
    if (APPLE)
        install(FILES LICENSE DESTINATION "${INS_PREF}" RENAME gpl)
    else() # specially encoded license file for Windows installer
        install(FILES packaging/nsis/LICENSE-utf16le DESTINATION "${INS_PREF}" RENAME gpl)
    endif()
endif()

if(APPLE)
    file(GLOB MIME_TRANS "${CMAKE_CURRENT_SOURCE_DIR}/packaging/mac/*.lproj" )
    install(DIRECTORY ${MIME_TRANS} DESTINATION ${INS_PREF})
endif()


file(GLOB snds "${CMAKE_CURRENT_SOURCE_DIR}/sounds/*.ogg")
list(APPEND snds "${CMAKE_CURRENT_SOURCE_DIR}/sounds/beat.raw48-16")
install(FILES ${snds} DESTINATION "${INS_PREF}/sounds")

# files related with Linux (mime, desktop entry, icons)
if(UNIX AND NOT APPLE)
    install(FILES mime/nootka.desktop DESTINATION share/applications)
    install(FILES mime/nootka.appdata.xml DESTINATION share/metainfo)
    install(FILES mime/nootka.xml DESTINATION share/mime/packages)
    if (NOT DPKG_CMD) # deb packager doesn't accept symbolic links
      install(FILES LICENSE DESTINATION "${INS_PREF}" RENAME gpl) # no gpl for Debian-s - they have copyright instead
    endif (NOT DPKG_CMD)
  # Icons in standard location
    install(FILES picts/hicolor/16x16/apps/nootka.png DESTINATION share/icons/hicolor/16x16/apps/)
    install(FILES picts/hicolor/24x24/apps/nootka.png DESTINATION share/icons/hicolor/24x24/apps/)
    install(FILES picts/hicolor/32x32/apps/nootka.png DESTINATION share/icons/hicolor/32x32/apps/)
    install(FILES picts/hicolor/48x48/apps/nootka.png DESTINATION share/icons/hicolor/48x48/apps/)
    install(FILES picts/hicolor/64x64/apps/nootka.png DESTINATION share/icons/hicolor/64x64/apps/)
    install(FILES picts/hicolor/128x128/apps/nootka.png DESTINATION share/icons/hicolor/128x128/apps/)
    install(FILES picts/hicolor/256x256/apps/nootka.png DESTINATION share/icons/hicolor/256x256/apps/)
    install(FILES picts/nootka.png DESTINATION share/icons/hicolor/512x512/apps/)
    install(FILES picts/hicolor/16x16/mimetypes/nootkaexam.png DESTINATION share/icons/hicolor/16x16/mimetypes/)
    install(FILES picts/hicolor/24x24/mimetypes/nootkaexam.png DESTINATION share/icons/hicolor/24x24/mimetypes/)
    install(FILES picts/hicolor/32x32/mimetypes/nootkaexam.png DESTINATION share/icons/hicolor/32x32/mimetypes/)
    install(FILES picts/hicolor/48x48/mimetypes/nootkaexam.png DESTINATION share/icons/hicolor/48x48/mimetypes/)
    install(FILES picts/hicolor/64x64/mimetypes/nootkaexam.png DESTINATION share/icons/hicolor/64x64/mimetypes/)
    install(FILES picts/hicolor/128x128/mimetypes/nootkaexam.png DESTINATION share/icons/hicolor/128x128/mimetypes/)
    install(FILES picts/nootka-exam.png DESTINATION share/icons/hicolor/256x256/mimetypes/ RENAME nootkaexam.png)
    install(FILES picts/hicolor/512x512/mimetypes/nootkaexam.png DESTINATION share/icons/hicolor/512x512/mimetypes/)
    install(FILES picts/hicolor/16x16/mimetypes/nootkalevel.png DESTINATION share/icons/hicolor/16x16/mimetypes/)
    install(FILES picts/hicolor/24x24/mimetypes/nootkalevel.png DESTINATION share/icons/hicolor/24x24/mimetypes/)
    install(FILES picts/hicolor/32x32/mimetypes/nootkalevel.png DESTINATION share/icons/hicolor/32x32/mimetypes/)
    install(FILES picts/hicolor/48x48/mimetypes/nootkalevel.png DESTINATION share/icons/hicolor/48x48/mimetypes/)
    install(FILES picts/hicolor/64x64/mimetypes/nootkalevel.png DESTINATION share/icons/hicolor/64x64/mimetypes/)
    install(FILES picts/hicolor/128x128/mimetypes/nootkalevel.png DESTINATION share/icons/hicolor/128x128/mimetypes/)
    install(FILES picts/nootka-level.png DESTINATION share/icons/hicolor/256x256/mimetypes/ RENAME nootkalevel.png)
    install(FILES picts/hicolor/512x512/mimetypes/nootkalevel.png DESTINATION share/icons/hicolor/512x512/mimetypes/)
    install(CODE "
        execute_process(COMMAND mkdir -p ${CMAKE_CURRENT_BINARY_DIR}/doc )
        execute_process(COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/packaging/make-chlog.sh ${Nootka_VERSION_FULL} ${CMAKE_CURRENT_BINARY_DIR}/doc/changelog ${CMAKE_CURRENT_SOURCE_DIR})
    ")
    if (DPKG_CMD) # Debian changelog
        install(FILES "${CMAKE_CURRENT_BINARY_DIR}/doc/changelog.Debian.gz" DESTINATION share/doc/nootka)
    endif ()
    install(FILES packaging/debian/copyright DESTINATION share/doc/nootka)
    install(FILES packaging/nootka.1.gz DESTINATION share/man/man1)
endif()

if(WIN32)
    get_filename_component(MAKE_DIR "${CMAKE_RC_COMPILER}" DIRECTORY) # it works ether with make and ninja
    install(FILES picts/nootka-exam.ico picts/nootka.ico picts/nootka-levels.ico picts/pack.ico picts/logo.bmp picts/logo-left.bmp DESTINATION "${INS_PREF}/picts") #ico
    install(FILES packaging/nsis/nootka-utf16.nsi packaging/nsis/fileasoc.nsh ${CMAKE_BINARY_DIR}/nsis/NSIS.definitions.nsh DESTINATION "${INS_PREF}")
    install(FILES packaging/nsis/qt.conf DESTINATION "${INS_PREF}")
    if (COPY_GCC_LIBS)
        install(FILES "${MAKE_DIR}/libgcc_s_dw2-1.dll" "${MAKE_DIR}/libstdc++-6.dll" "${MAKE_DIR}/libwinpthread-1.dll" DESTINATION "${INS_PREF}")
    endif()
endif()

###########################################################################
# uninstall target
###########################################################################
if(NOT ANDROID)
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY
    )

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
    )
endif()

add_subdirectory(lang)

add_subdirectory(packaging) # deploy targets, cpack rules


# Uncomment below lines if you want to build any testing executable
# add_subdirectory( tests/nootoxml )
# add_subdirectory( tests/bp )



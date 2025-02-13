##########################################################################################
# CMakeList.txt packaging
#
#  packaging targets (make src)
##########################################################################################
if(UNIX AND NOT APPLE)
    message(STATUS
        "\nTo build source package, call:
         make src\n"
    )
    add_custom_target(src
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/src/make-src.sh ${Nootka_VERSION_FULL} ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}
    )
ENDIF()

###########################################################################
#  installing target (make runinplace) - copy or link necessary files to run Nootka without install
###########################################################################
message(STATUS
    "\nTo enable launching nootka without installing, call:
     make runinplace\nBinary nootka is in src folder, so launch it invoking: ./src/nootka\n"
)

find_package(Qt6Core)
get_target_property(QtCore_location_Release Qt6::Core LOCATION_Release)
get_filename_component(QT_BINARY_DIR "${QtCore_location_Release}" DIRECTORY)

if(WIN32)
    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/nsis/definitions.nsh.in
        ${CMAKE_BINARY_DIR}/nsis/NSIS.definitions.nsh
    )

    get_filename_component(VORBIS_DIR "${VORBIS_LIBRARIES}" DIRECTORY)
    get_filename_component(MAKE_DIR "${CMAKE_RC_COMPILER}" DIRECTORY)

    add_custom_target(deploy
        COMMAND echo "deploying..."
        COMMAND ${QT_BINARY_DIR}/windeployqt.exe ${CMAKE_INSTALL_PREFIX}/nootka.exe ${CMAKE_INSTALL_PREFIX}/libNootkaSound.dll --no-translations --no-svg --qmldir "${CMAKE_SOURCE_DIR}/src/qml"
    )
else()
    if (APPLE)
        set(LINK_PATH "${CMAKE_BINARY_DIR}/Resources")
    else ()
        set(LINK_PATH "${CMAKE_BINARY_DIR}")
    endif ()
    add_custom_target(runinplace
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/make-noinstall.sh ${CMAKE_SOURCE_DIR} ${LINK_PATH}
    )
endif()

if (APPLE)
    add_custom_target(deploy
        COMMAND echo "deploying..."
        COMMAND echo ${QT_BINARY_DIR}/../../bin
        COMMAND ${QT_BINARY_DIR}/../../bin/macdeployqt ${CMAKE_INSTALL_PREFIX}/nootka.app -qmldir=${CMAKE_SOURCE_DIR}/src/qml
    )

    add_custom_target(dmg
        COMMAND echo "creating dmg image..."
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/mac/create-dmg.sh ${Nootka_VERSION_FULL} ${CMAKE_SOURCE_DIR} ${CMAKE_INSTALL_PREFIX}
    )
endif()

# if (UNIX AND NOT APPLE)
  # appimage target
  # To correctly generate AppImage set install prefix to '/usr'
  # and when using with older Linux system (i.e. Ubuntu Trusty 14.04)
  # call
  # cmake with -DQT_QMAKE_EXECUTABLE=/opt/qtXX/bin/qmake
  # add_custom_target(appimage
    # COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/appimage/make-appimage.sh ${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${QT_QMAKE_EXECUTABLE} ${Nootka_VERSION_FULL}
  # )
# endif()

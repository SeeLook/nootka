project(nootka)
cmake_minimum_required(VERSION 2.8)
find_package(Qt4 REQUIRED)
INCLUDE( ${QT_USE_FILE} )
add_definitions(${QT_DEFINITIONS})

set(CMAKE_COLOR_MAKEFILE ON)

include_directories(${QT_INCLUDES})

add_subdirectory(src)


#INSTALLATION
if(UNIX AND NOT APPLE)
#     INSTALL(TARGETS nootka DESTINATION bin)
    INSTALL(DIRECTORY picts DESTINATION share/nootka)
    INSTALL(DIRECTORY fonts DESTINATION share/nootka)

    # uninstall target
    configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

    add_custom_target(uninstall
        COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif(UNIX AND NOT APPLE)


# add_subdirectory(picts)

########################################################################
# MAIN CMAKE FILE FOR Nootka
########################################################################
 
cmake_minimum_required(VERSION 2.8.11)

project(nootka)

SET(Nootka_VERSION_MAJOR  "1")
SET(Nootka_VERSION_MINOR  "1")
SET(Nootka_VERSION_PATCH  "1-alpha")

SET(Nootka_VERSION       "${Nootka_VERSION_MAJOR}.${Nootka_VERSION_MINOR}")
SET(Nootka_VERSION_FULL  "${Nootka_VERSION}.${Nootka_VERSION_PATCH}")
message(STATUS "
${PROJECT_NAME}-${Nootka_VERSION_FULL}
" )


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/src)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/src) 


add_subdirectory(src)

###########################################################################
# uninstall target
###########################################################################
configure_file(
     "${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
     "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)

  add_custom_target(uninstall
   COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake
)

add_subdirectory(packaging) # cpack rules
project(RecordItNow)


set(BUILD_WITH_JOSCHY true)
if (BUILD_WITH_JOSCHY)
    add_subdirectory(joschy-snapshot-23-02-10)
endif (BUILD_WITH_JOSCHY)

find_package(KDE4 REQUIRED)
include (KDE4Defaults)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})


set(KDE_MIN_VERSION "4.4.0")

########### install files ###############
install(FILES recorditnow.desktop  DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES recorditnow.notifyrc DESTINATION ${DATA_INSTALL_DIR}/recorditnow)

if (NOT BUILD_WITH_JOSCHY)
    find_package(JoschyCore REQUIRED)
else (NOT BUILD_WITH_JOSCHY)
    set (JOSCHY_SNAPSHOT_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/joschy-snapshot-23-02-10/joschycore/)
    set (JOSCHY_SNAPSHOT_LIBRARIES joschycore)
endif (NOT BUILD_WITH_JOSCHY)


find_package(RecordMyDesktop REQUIRED)

add_subdirectory(src)


# translation
 MACRO(LIST_CONTAINS var value)
  SET(${var})
  FOREACH (value2 ${ARGN})
    IF (${value} STREQUAL ${value2})
      SET(${var} TRUE)
    ENDIF (${value} STREQUAL ${value2})
  ENDFOREACH (value2)
ENDMACRO(LIST_CONTAINS)

STRING(REPLACE " " ";" LINGUAS "${LINGUAS}")
FIND_PROGRAM(GETTEXT_MSGFMT_EXECUTABLE msgfmt)
IF(NOT GETTEXT_MSGFMT_EXECUTABLE)
        MESSAGE(
"------
                 NOTE: msgfmt not found. Translations will *not* be installed
------")
ELSE(NOT GETTEXT_MSGFMT_EXECUTABLE)
        SET(catalogname recorditnow)
        ADD_CUSTOM_TARGET(translations ALL)
        FILE(GLOB PO_FILES po/${catalogname}*.po)
        FOREACH(_poFile ${PO_FILES})
                GET_FILENAME_COMPONENT(_poFileName ${_poFile} NAME)
                STRING(REGEX REPLACE "^${catalogname}_?" "" _langCode ${_poFileName} )
                STRING(REGEX REPLACE "\\.po$" "" _langCode ${_langCode} )
                LIST_CONTAINS(INSTALL_LANG ${_langCode} ${LINGUAS} )
                IF( INSTALL_LANG )
                        MESSAGE(STATUS "Install translation: "${_langCode})
                        GET_FILENAME_COMPONENT(_lang ${_poFile} NAME_WE)
                        SET(_gmoFile ${CMAKE_CURRENT_BINARY_DIR}/${_lang}.gmo)
                        ADD_CUSTOM_COMMAND(TARGET translations
                                COMMAND ${GETTEXT_MSGFMT_EXECUTABLE} --check -o ${_gmoFile} ${_poFile}
                                DEPENDS ${_poFile})
                        INSTALL(FILES ${_gmoFile} DESTINATION ${LOCALE_INSTALL_DIR}/${_langCode}/LC_MESSAGES/ RENAME ${catalogname}.mo)
                ENDIF( INSTALL_LANG )
        ENDFOREACH(_poFile ${PO_FILES})
ENDIF(NOT GETTEXT_MSGFMT_EXECUTABLE)

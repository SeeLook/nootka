########################################################################
######   Cmake rules for creating Nootka translation files *.qm   ######
########################################################################

find_package(Qt5 COMPONENTS LinguistTools)

file(GLOB TS_FILES "nootka_*.ts")
list(APPEND TS_FILES "qtbase_sl.ts")
list(APPEND TS_FILES "qtbase_pt.ts")

#list(REMOVE_ITEM ${TS_FILES} "if_any")

qt5_add_translation(QM_FILES ${TS_FILES})

# HACK: above command doesn't want to invoke itself.
# Following trick is needed
add_custom_target(translations ALL DEPENDS ${QM_FILES})

if(APPLE OR WIN32)
  find_package(Qt5Core)
  get_target_property(QtCore_location_Release Qt5::Core LOCATION_Release)
  get_filename_component(QT_BINARY_DIR "${QtCore_location_Release}" DIRECTORY)
  if (WIN32)
    if(NOT DEFINED QT_TRANSLATIONS_DIR)
      set(QT_TRANSLATIONS_DIR "${QT_BINARY_DIR}/../translations")
    endif()
  else (WIN32)
    set(QT_TRANSLATIONS_DIR "${QT_BINARY_DIR}/../../translations")
  endif (WIN32)
  list(APPEND QM_FILES
    "${QT_TRANSLATIONS_DIR}/qtbase_cs.qm"
    "${QT_TRANSLATIONS_DIR}/qtbase_de.qm"
    "${QT_TRANSLATIONS_DIR}/qtbase_es.qm"
    "${QT_TRANSLATIONS_DIR}/qtbase_fr.qm"
    "${QT_TRANSLATIONS_DIR}/qtbase_hu.qm"
    "${QT_TRANSLATIONS_DIR}/qtbase_it.qm"
    "${QT_TRANSLATIONS_DIR}/qtbase_pl.qm"
    # "${QT_TRANSLATIONS_DIR}/qtbase_pt_BR.qm"
    "${QT_TRANSLATIONS_DIR}/qtbase_ru.qm"
    "${QT_TRANSLATIONS_DIR}/qtbase_uk.qm"
    #"${QT_TRANSLATIONS_DIR}/qtbase_sl.qm"
    # So far not provided by Qt
    #"${CMAKE_CURRENT_SOURCE_DIR}/lang/qtbase/qtbase_sl.qm"
  )
endif(APPLE OR WIN32)

install(FILES ${QM_FILES} DESTINATION "${INS_PREF}/lang")

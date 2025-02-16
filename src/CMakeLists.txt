########################################################################
# CMakeList.txt src
########################################################################

find_package(Qt6 REQUIRED COMPONENTS Widgets PrintSupport Qml QuickControls2 Core5Compat)
if (ANDROID)
    find_package(Qt6 REQUIRED COMPONENTS Multimedia)
endif()

qt_standard_project_setup(REQUIRES 6.5)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(QT_QML_GENERATE_QMLLS_INI ON)

if(UNIX AND NOT APPLE)
    if (ANDROID)
        if (CMAKE_ANDROID_ARCH_ABI MATCHES "arm64-v8a" OR CMAKE_ANDROID_ARCH_ABI MATCHES "armeabi")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsigned-char")
            message("Set additional flag -fsigned-char for ${CMAKE_ANDROID_ARCH_ABI} arch")
        endif()
    else()
        if (PACKAGE_ARCHITECTURE MATCHES "arm" OR PACKAGE_ARCHITECTURE MATCHES "aarch64")
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsigned-char")
            message("Set additional flag -fsigned-char for ${PACKAGE_ARCHITECTURE} arch")
        endif()
    endif()
endif()

if(APPLE) # Mac RPath
    set(CMAKE_MACOSX_RPATH 1)
    set(CMAKE_INSTALL_RPATH "@loader_path/../Frameworks")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DQT_QML_DEBUG)
endif ()

qt_add_executable(nootka WIN32)

add_subdirectory(core)
add_subdirectory(sound)

include_directories(core sound)

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
    main/tgotit.cpp

    dialogs/tdialogloaderobject.cpp
    dialogs/tlevelcreatoritem.cpp
    dialogs/tlevelpreviewitem.cpp
    dialogs/tlevelsdefs.cpp
    dialogs/tlevelselector.cpp
    dialogs/tmelodylistview.cpp
    dialogs/tmelodywrapper.cpp
    dialogs/ttunerdialogitem.cpp
    dialogs/trtmselectoritem.cpp
    dialogs/tmelgenitem.cpp
    dialogs/tmelodyimportitem.cpp

    help/texamhelp.cpp
    help/tmainhelp.cpp
)

if(MINGW)
    add_custom_command(
        OUTPUT
            ${CMAKE_CURRENT_BINARY_DIR}/nootico.o
        COMMAND
            windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/../picts/nootka-icon.rc -o ${CMAKE_CURRENT_BINARY_DIR}/nootico.o
    )
    set(NOOTKA_SRCS ${NOOTKA_SRCS} ${CMAKE_CURRENT_BINARY_DIR}/nootico.o)
    set(NOOTKA_EXE_ICON nootico.o)
endif()

if(ANDROID)
    list(APPEND NOOTKA_SRC
        mobile/tmobilemenu.cpp
    )
    qt_add_resources(NOOTKA_SRC nootka-android.qrc)
else()
    list(APPEND NOOTKA_SRC
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
    qt_add_resources(NOOTKA_SRC nootka.qrc)
endif()

option(ENABLE_UPDATER "Enable online checking for new Nootka versions" ON)
if(ENABLE_UPDATER)
    message(STATUS "check for updates\t\t TRUE")
    find_package(Qt6Network)
    list(APPEND NOOTKA_SRC
        updater/tupdatechecker.cpp
        updater/updatefunctions.cpp
        updater/tupdateitem.cpp
    )
else()
    message(STATUS "check for updates\t\t FALSE")
endif()

include_directories(core/music core/score)
qt_add_qml_module(nootka
    URI "Nootka.Music"
    SOURCES
        core/taction.cpp
        core/taction.h

        core/music/tnote.h
        core/music/tnote.cpp
        core/music/trhythm.h
        core/music/trhythm.cpp
        core/music/tclef.h
        core/music/tclef.cpp
        core/music/tmeter.h
        core/music/tmeter.cpp
        core/music/ttune.h
        core/music/ttune.cpp
        core/music/tinstrument.h
        core/music/tinstrument.cpp

        core/score/tscoreobject.cpp
        core/score/tscoreobject.h
        core/score/tstaffitem.cpp
        core/score/tstaffitem.h
        core/score/tmeasureobject.cpp
        core/score/tmeasureobject.h
        core/score/tnoteitem.cpp
        core/score/tnoteitem.h
        core/score/tnotepair.cpp
        core/score/tnotepair.h
        core/score/tbeamobject.cpp
        core/score/tbeamobject.h
        core/score/tstafflines.cpp
        core/score/tstafflines.h
        core/score/taddnoteitem.cpp
        core/score/taddnoteitem.h
        core/score/tmelodypreview.cpp
        core/score/tmelodypreview.h
        core/score/tdummychord.cpp
        core/score/tdummychord.h
    DEPENDENCIES
        QtQuick
)

# qmlRegisterUncreatableType<TmelodyPart>("Score", 1, 0, "TmelodyPart", QStringLiteral("You cannot create an instance of the TcommonInstrument."));

target_sources(nootka PUBLIC
    ${NOOTKA_SRC}
    ${NOOTKA_EXE_ICON}
)

target_link_libraries(nootka PUBLIC
    Qt6::Core
    Qt6::Widgets
    Qt6::Qml
    Qt6::Quick
    Qt6::QuickControls2
    Qt6::PrintSupport
    Qt6::Core5Compat
)

if (ANDROID)
    target_link_libraries(nootka PUBLIC
        Qt6::Multimedia
    )

    set_target_properties(nootka PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android"
        QT_ANDROID_PACKAGE_NAME "net.sf.nootka"
        QT_ANDROID_TARGET_SDK_VERSION 35
        QT_ANDROID_VERSION_CODE 72
        QT_ANDROID_VERSION_NAME ${Nootka_VERSION_FULL}
    )

    qt_import_plugins(nootka
        # INCLUDE_BY_TYPE imageformats Qt::QSvgPlugin
        EXCLUDE_BY_TYPE qmltooling
        EXCLUDE_BY_TYPE iconengines
        # EXCLUDE_BY_TYPE networkinformation
        EXCLUDE_BY_TYPE tls
        EXCLUDE_BY_TYPE platforminputcontexts
    )
endif()

if(UNIX AND NOT APPLE) # Linux path for Nootka binary
    install(TARGETS nootka DESTINATION bin)
else()
    if(WIN32) # Windows
        install(TARGETS nootka DESTINATION .)
    else() # MacOs
        install(TARGETS nootka DESTINATION "${CMAKE_INSTALL_PREFIX}/nootka.app/Contents/MacOs")
        install(CODE "
            execute_process(COMMAND echo copying info.plist )
            execute_process(COMMAND sh ${CMAKE_SOURCE_DIR}/packaging/mac/grabInfo.sh ${Nootka_VERSION_FULL} ${CMAKE_SOURCE_DIR} ${CMAKE_INSTALL_PREFIX} )"
        )
    endif()
endif()


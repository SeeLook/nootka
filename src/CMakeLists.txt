set(nootka_SRCS
    main.cpp
    mainwindow.cpp
    tglobals.cpp
    tscorewidget.cpp
    tscorewidgetsimple.cpp
    tkeysignatureview.cpp
    tnote.cpp
    tnoteview.cpp
    tscorewidgetsimple.cpp
    scoresetttingsdlg.cpp
)

# qt4_wrap_cpp(nootka_SRCS mainwindow.h tscorewidget.h tglobals.h)
qt4_wrap_cpp(nootka_SRCS mainwindow.h)
add_executable(nootka ${nootka_SRCS})
target_link_libraries(nootka ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTSVG_LIBRARY})

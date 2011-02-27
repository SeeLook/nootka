set(nootka_SRCS
    main.cpp
    mainwindow.cpp
)

qt4_wrap_cpp(nootka_SRCS mainwindow.h)
add_executable(nootka ${nootka_SRCS})
target_link_libraries(nootka ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

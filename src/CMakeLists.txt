set(nootka_SRCS
    main.cpp
)

qt4_wrap_cpp(nootka_SRCS MainWindow.h)
add_executable(nootka ${nootka_SRCS})
target_link_libraries(nootka ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY})

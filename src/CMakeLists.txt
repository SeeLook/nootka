set(nootka_SRCS
    main.cpp
)

add_executable(nootka ${nootka_SRCS})

target_link_libraries(nootka ${QT_QTCORE_LIBRARY})

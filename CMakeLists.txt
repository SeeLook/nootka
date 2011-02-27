project(nootka)
cmake_minimum_required(VERSION 2.8)

find_package(Qt4 REQUIRED)

add_definitions(${QT_DEFINITIONS})
include_directories(${QT_INCLUDES})

add_subdirectory(src)

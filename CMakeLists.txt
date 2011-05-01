project(unity-2d)
cmake_minimum_required(VERSION 2.8)

# Dirs
set(UNITY_2D_DIR share/unity-2d)
set(UNITY_DIR /usr/share/unity/)

configure_file(config.h.in config.h)

# Build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual -Wall -std=c++0x")

# Dependencies
include(FindPkgConfig)
find_package(Qt4 REQUIRED)
find_package(X11 REQUIRED)
find_package(Gettext REQUIRED)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${QT_INCLUDE_DIR}
    ${QT_QTCORE_INCLUDE_DIR}
    ${QT_QTDBUS_INCLUDE_DIR}
    ${QT_QTDECLARATIVE_INCLUDE_DIR}
    ${QT_QTXML_INCLUDE_DIR}
    ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTOPENGL_INCLUDE_DIR}
    ${X11_INCLUDE_DIR}
    )

# Tests
add_custom_target(check)

# Source
add_subdirectory(libunity-2d-private)
add_subdirectory(launcher)
add_subdirectory(panel)
add_subdirectory(places)
add_subdirectory(spread)
add_subdirectory(po)

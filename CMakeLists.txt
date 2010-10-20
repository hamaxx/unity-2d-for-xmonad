project(unity-qt)
cmake_minimum_required(VERSION 2.8)

# Dirs
set(UNITY_QT_DIR share/unity-qt)
set(THEME_DIR share/unity/themes)
set(AUTOSTART_DIR share/autostart CACHE PATH "The directory where autostart files should be installed")

configure_file(config.h.in config.h)

# Build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual -Wall -std=c++0x")

# Dependencies
include(FindPkgConfig)
find_package(Qt4 REQUIRED)

include_directories(
    ${CMAKE_BINARY_DIR}
    ${QT_INCLUDE_DIR}
    ${QT_QTCORE_INCLUDE_DIR}
    ${QT_QTDBUS_INCLUDE_DIR}
    ${QT_QTDECLARATIVE_INCLUDE_DIR}
    ${QT_QTXML_INCLUDE_DIR}
    ${QT_QTGUI_INCLUDE_DIR}
    )

# Source
add_subdirectory(launcher)
add_subdirectory(panel)
add_subdirectory(places)

# Install
install(DIRECTORY themes/
    DESTINATION ${THEME_DIR}
    )

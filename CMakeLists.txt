project(unity-qt)
cmake_minimum_required(VERSION 2.8)

# Dirs
set(UNITY_QT_DIR share/unity-qt)
set(AUTOSTART_DIR share/autostart CACHE PATH "The directory where autostart files should be installed")

configure_file(config.h.in config.h)

# Dependencies
include(FindPkgConfig)
find_package(Qt4 REQUIRED)

# Source
add_subdirectory(launcher)

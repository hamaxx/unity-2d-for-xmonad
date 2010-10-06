project(unity-qt-launcher)

cmake_minimum_required(VERSION 2.8)

# Dirs
set(UNITY_QT_DIR share/unity-qt)
set(AUTOSTART_DIR share/autostart CACHE PATH "The directory where autostart files should be installed")

# Dependencies
include(FindPkgConfig)
find_package(Qt4 REQUIRED)

# Source
add_subdirectory(UnityApplications)
add_subdirectory(app)

file(GLOB launcher_QML *.qml)

configure_file(config.h.in config.h)

# Install
install(FILES ${launcher_QML}
    DESTINATION ${UNITY_QT_DIR}
    )

install(DIRECTORY artwork
    DESTINATION ${UNITY_QT_DIR}
    )

install(FILES unity-qt-launcher.desktop
    DESTINATION ${AUTOSTART_DIR}
    )

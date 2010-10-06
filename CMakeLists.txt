project(unity-qt-launcher)

cmake_minimum_required(VERSION 2.8)

# Dependencies
include(FindPkgConfig)
find_package(Qt4 REQUIRED)

# Source
add_subdirectory(UnityApplications)
add_subdirectory(launcher)

file(GLOB launcher_QML *.qml)

# Install
set (UNITY_QT_DIR share/unity-qt)
# FIXME: hardcoded
set (AUTOSTART_DIR /etc/xdg/xdg-unity-qt/autostart)

install(FILES ${launcher_QML}
    DESTINATION ${UNITY_QT_DIR}
    )

install(DIRECTORY artwork
    DESTINATION ${UNITY_QT_DIR}
    )

install(FILES unity-qt-launcher.desktop
    DESTINATION ${AUTOSTART_DIR}
    )

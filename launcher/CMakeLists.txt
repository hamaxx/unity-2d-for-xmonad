project(unity-qt-launcher)

# Source
add_subdirectory(UnityApplications)
add_subdirectory(app)

file(GLOB launcher_QML *.qml)

# Install
install(FILES ${launcher_QML}
    DESTINATION ${UNITY_QT_DIR}/launcher
    )

install(FILES artwork/devices.png artwork/glow.png artwork/shadow.png
    DESTINATION ${UNITY_QT_DIR}/launcher/artwork
    )

install(FILES artwork/applications.png artwork/files.png
    DESTINATION ${UNITY_QT_DIR}
    )

install(FILES unity-qt-launcher.desktop
    DESTINATION ${AUTOSTART_DIR}
    )

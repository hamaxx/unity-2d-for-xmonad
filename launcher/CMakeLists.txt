project(unity-qt-launcher)

# Source
add_subdirectory(UnityApplications)
add_subdirectory(app)

file(GLOB launcher_QML *.qml)

# Install
install(FILES ${launcher_QML}
    DESTINATION ${UNITY_QT_DIR}/launcher
    )

install(DIRECTORY artwork
    DESTINATION ${UNITY_QT_DIR}/launcher
    )

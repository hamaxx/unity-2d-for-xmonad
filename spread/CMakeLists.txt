project(unity-qt-spread)

# Source
add_subdirectory(app)

file(GLOB spread_QML *.qml)

# Install
install(FILES ${spread_QML}
    DESTINATION ${UNITY_QT_DIR}/spread
    )

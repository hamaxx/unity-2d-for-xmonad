project(unity-2d-spread)

# Source
add_subdirectory(app)

file(GLOB spread_QML *.qml)

# Install
install(FILES ${spread_QML}
    DESTINATION ${UNITY_2D_DIR}/spread
    )

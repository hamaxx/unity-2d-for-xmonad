project(unity-2d-launcher)

include_directories(
    ${libunity-2d-private_SOURCE_DIR}/src
    )

# Source
add_subdirectory(app)
add_subdirectory(tests)

file(GLOB launcher_QML *.qml)

# Install
install(FILES ${launcher_QML}
    DESTINATION ${UNITY_2D_DIR}/launcher
    )

install(DIRECTORY artwork
    DESTINATION ${UNITY_2D_DIR}/launcher
    )

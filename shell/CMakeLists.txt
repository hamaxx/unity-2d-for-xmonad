project(unity-2d-shell)

# Source
add_subdirectory(app)

# Install
install(DIRECTORY qml
    DESTINATION ${UNITY_2D_DIR}/shell
    )

install(DIRECTORY artwork
    DESTINATION ${UNITY_2D_DIR}/shell
    )

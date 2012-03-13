project(unity-2d-shell)

# Source
add_subdirectory(app)
add_subdirectory(tests)

file(GLOB shell_QML *.qml *.js)

# Install
install(FILES ${shell_QML}
    DESTINATION ${UNITY_2D_DIR}/shell
    )

install(DIRECTORY artwork
    DESTINATION ${UNITY_2D_DIR}/shell
    )

install(DIRECTORY common
    DESTINATION ${UNITY_2D_DIR}/shell
    )

install(DIRECTORY launcher
    DESTINATION ${UNITY_2D_DIR}/shell
    )

install(DIRECTORY dash
    DESTINATION ${UNITY_2D_DIR}/shell
    )

install(DIRECTORY hud
    DESTINATION ${UNITY_2D_DIR}/shell
    )

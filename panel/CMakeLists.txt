project(unity-2d-panel)

# Source
include_directories(
    ${libunity-2d_SOURCE_DIR}/src
    )

add_subdirectory(applets)
add_subdirectory(app)
add_subdirectory(tests)

# Install
install(DIRECTORY artwork
    DESTINATION ${UNITY_2D_DIR}/panel
    )

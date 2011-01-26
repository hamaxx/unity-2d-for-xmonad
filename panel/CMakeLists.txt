project(unity-2d-panel)

# Source
include_directories(
    ${libunity-2d_SOURCE_DIR}/src
    )

add_subdirectory(applets)
add_subdirectory(app)
add_subdirectory(tests)

# Tests
add_custom_target(check DEPENDS launcher_check)

# Install
install(DIRECTORY artwork
    DESTINATION ${UNITY_2D_DIR}/panel
    )

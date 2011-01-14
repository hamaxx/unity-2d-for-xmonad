project(unity-2d-panel)

# Source
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    )

add_subdirectory(lib)
add_subdirectory(applets)
add_subdirectory(app)
add_subdirectory(tests)

# Tests
add_custom_target(check DEPENDS panel_check launcher_check)

# Install
install(DIRECTORY artwork
    DESTINATION ${UNITY_2D_DIR}/panel
    )

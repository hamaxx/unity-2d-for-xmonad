project(unity-2d-panel)

# Dependencies
pkg_check_modules(UNITYCORE REQUIRED unity-core-4.0)

# Source
include_directories(
    ${libunity-2d-private_SOURCE_DIR}/src
    )

add_subdirectory(applets)
add_subdirectory(app)
add_subdirectory(tests)

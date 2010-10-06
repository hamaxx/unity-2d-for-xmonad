project(unity-qt-panel)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
    )

add_subdirectory(lib)
add_subdirectory(applets)
add_subdirectory(app)
add_subdirectory(tests)

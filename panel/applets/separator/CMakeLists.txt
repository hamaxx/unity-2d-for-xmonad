project(panelplugin-separator)

# Sources
set(separator_SRCS
    separatorapplet.cpp
    plugin.cpp
    )

# Build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

qt4_automoc(${separator_SRCS})
add_library(panelplugin-separator SHARED ${separator_SRCS})
set_target_properties(panelplugin-separator PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ".."
    VERSION 0
    SOVERSION 0.0
    )

target_link_libraries(panelplugin-separator
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    unity-2d-private
    )

install(TARGETS panelplugin-separator
        LIBRARY DESTINATION lib/unity-2d/plugins/panel
       )

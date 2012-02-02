project(panelplugin-xmonadlog)

# Sources
set(xmonadlog_SRCS
    xmonadlogapplet.cpp
    plugin.cpp
    )

# Build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

qt4_automoc(${xmonadlog_SRCS})
add_library(panelplugin-xmonadlog SHARED ${xmonadlog_SRCS})
set_target_properties(panelplugin-xmonadlog PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ".."
    VERSION 0
    SOVERSION 0.0
    )

target_link_libraries(panelplugin-xmonadlog
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    unity-2d-private
    )

install(TARGETS panelplugin-xmonadlog
        LIBRARY DESTINATION lib/unity-2d/plugins/panel
       )

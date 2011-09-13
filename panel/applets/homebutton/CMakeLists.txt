project(panelplugin-homebutton)

# Sources
set(homebutton_SRCS
    homebuttonapplet.cpp
    homebutton.cpp
    plugin.cpp
    )

# Build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

qt4_automoc(${homebutton_SRCS})
add_library(panelplugin-homebutton SHARED ${homebutton_SRCS})
set_target_properties(panelplugin-homebutton PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ".."
    VERSION 0
    SOVERSION 0.0
    )

target_link_libraries(panelplugin-homebutton
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    unity-2d-private
    )

install(TARGETS panelplugin-homebutton
        LIBRARY DESTINATION lib/unity-2d/plugins/panel
       )

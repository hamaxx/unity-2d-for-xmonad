project(panelplugin-homebutton)

# Sources
set(uqapplets_SRCS
    homebuttonapplet.cpp
    homebutton.cpp
    plugin.cpp
    )

# Build
include_directories(
    ${libunity-2d-private_SOURCE_DIR}/src
    )

qt4_automoc(${uqapplets_SRCS})
add_library(panelplugin-homebutton SHARED ${uqapplets_SRCS})
set_target_properties(panelplugin-homebutton PROPERTIES LIBRARY_OUTPUT_DIRECTORY "..")

target_link_libraries(panelplugin-homebutton
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    unity-2d-private
    uqapplets
    )


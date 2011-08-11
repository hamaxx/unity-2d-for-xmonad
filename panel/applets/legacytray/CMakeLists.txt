project(panelplugin-legacytray)

# Sources
set(legacytray_SRCS
    legacytrayapplet.cpp
    fdoselectionmanager.cpp
    fdotask.cpp
    x11embedcontainer.cpp
    x11embeddelegate.cpp
    x11embedpainter.cpp
    plugin.cpp
    )

# Build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${X11_INCLUDE_DIR}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

qt4_automoc(${legacytray_SRCS})
add_library(panelplugin-legacytray SHARED ${legacytray_SRCS})
set_target_properties(panelplugin-legacytray PROPERTIES LIBRARY_OUTPUT_DIRECTORY "..")

target_link_libraries(panelplugin-legacytray
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${X11_LIBRARIES}
    ${X11_Xrender_LIB}
    ${X11_Xcomposite_LIB}
    ${X11_Xdamage_LIB}
    ${X11_Xfixes_LIB}
    unity-2d-private
    uqapplets
    )

install(TARGETS panelplugin-legacytray
        LIBRARY DESTINATION lib/unity-2d/plugins/panel
       )

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

add_definitions(-DWNCK_I_KNOW_THIS_IS_UNSTABLE)

# Build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${DCONFQT_INCLUDE_DIRS}
    ${WNCK_INCLUDE_DIRS}
    ${X11_INCLUDE_DIR}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

qt4_automoc(${legacytray_SRCS})
add_library(panelplugin-legacytray SHARED ${legacytray_SRCS})
set_target_properties(panelplugin-legacytray PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ".."
    VERSION 0
    SOVERSION 0.0
    )

target_link_libraries(panelplugin-legacytray
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${DCONFQT_LIBRARIES}
    ${WNCK_LIBRARIES}
    ${X11_LIBRARIES}
    ${X11_Xrender_LIB}
    ${X11_Xcomposite_LIB}
    ${X11_Xdamage_LIB}
    ${X11_Xfixes_LIB}
    unity-2d-private
    )

install(TARGETS panelplugin-legacytray
        LIBRARY DESTINATION lib/unity-2d/plugins/panel
       )

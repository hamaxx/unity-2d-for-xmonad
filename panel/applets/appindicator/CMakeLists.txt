project(panelplugin-appindicator)

# Dependencies
pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)

# Sources
set(appindicator_SRCS
    appindicatorapplet.cpp
    sniitem.cpp
    plugin.cpp
    )

# Build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${DBUSMENUQT_INCLUDE_DIRS}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

qt4_automoc(${appindicator_SRCS})
add_library(panelplugin-appindicator SHARED ${appindicator_SRCS})
set_target_properties(panelplugin-appindicator PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ".."
    VERSION 0
    SOVERSION 0.0
    )

target_link_libraries(panelplugin-appindicator
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${DBUSMENUQT_LIBRARIES}
    unity-2d-private
    )

install(TARGETS panelplugin-appindicator
        LIBRARY DESTINATION lib/unity-2d/plugins/panel
       )

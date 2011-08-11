project(panelplugin-appindicator)

# Dependencies
include(FindPkgConfig)

pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)
pkg_check_modules(INDICATOR REQUIRED indicator)

# Sources
set(appindicator_SRCS
    appindicatorapplet.cpp
    sniitem.cpp
    plugin.cpp
    )

# Build
include_directories(
    ${DBUSMENUQT_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    ${INDICATOR_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

qt4_automoc(${appindicator_SRCS})
add_library(panelplugin-appindicator SHARED ${appindicator_SRCS})
set_target_properties(panelplugin-appindicator PROPERTIES LIBRARY_OUTPUT_DIRECTORY "..")

target_link_libraries(panelplugin-appindicator
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${DBUSMENUQT_LDFLAGS}
    ${GTK_LDFLAGS}
    ${INDICATOR_LDFLAGS}
    unity-2d-private
    uqapplets
    )


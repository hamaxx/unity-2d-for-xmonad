project(panelplugin-indicator)

macro(read_pkg_variable cmake_var pkg pkg_var)
    execute_process(
        COMMAND pkg-config --variable=${pkg_var} ${pkg}
        OUTPUT_VARIABLE tmp
        )
    # Remove trailing newline from ${tmp}
    string(STRIP "${tmp}" ${cmake_var})
endmacro(read_pkg_variable)

# Dependencies
pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)
pkg_check_modules(INDICATOR REQUIRED indicator)

# Get indicator dirs from pkgconfig
read_pkg_variable(INDICATOR_DIR       indicator indicatordir)
read_pkg_variable(INDICATOR_ICONS_DIR indicator iconsdir)
configure_file(indicator-config.h.in indicator-config.h)

# Sources
set(indicator_SRCS
    abstractindicator.cpp
    datetimeindicator.cpp
    indicatorapplet.cpp
    indicatorservicemanager.cpp
    indicator.c
    plugin.cpp
    )

# Build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${DBUSMENUQT_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    ${INDICATOR_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

qt4_automoc(${indicator_SRCS})
add_library(panelplugin-indicator SHARED ${indicator_SRCS})
set_target_properties(panelplugin-indicator PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ".."
    VERSION 0
    SOVERSION 0.0
    )

target_link_libraries(panelplugin-indicator
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${DBUSMENUQT_LDFLAGS}
    ${GTK_LDFLAGS}
    ${INDICATOR_LDFLAGS}
    unity-2d-private
    )

install(TARGETS panelplugin-indicator
        LIBRARY DESTINATION lib/unity-2d/plugins/panel
       )

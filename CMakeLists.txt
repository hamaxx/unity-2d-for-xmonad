project(unity-2d)
cmake_minimum_required(VERSION 2.8)

# Dirs
set(UNITY_2D_DIR share/unity-2d)
set(UNITY_DIR /usr/share/unity/)
set(UNITY_2D_DATA_DIR "${CMAKE_SOURCE_DIR}/data")

configure_file(config.h.in config.h)

# Build flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual -Wall -std=c++0x")
# Flags to help detecting use of deprecated GTK APIs and direct access to object fields
# https://live.gnome.org/GnomeGoals/RemoveDeprecatedSymbols/GTK+
# https://live.gnome.org/GnomeGoals/UseGseal
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGSEAL_ENABLE -DGDK_PIXBUF_DISABLE_DEPRECATED -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED")

# gtk and unity-core (actually sigc++) do not like Qt defining macros named
# "signals" and "slots"
add_definitions(-DQT_NO_KEYWORDS)

# Dependencies
include(FindPkgConfig)
find_package(Qt4 REQUIRED)
find_package(X11 REQUIRED)
find_package(Gettext REQUIRED)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GDK REQUIRED gdk-3.0)
pkg_check_modules(GTK REQUIRED gtk+-3.0)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(WNCK REQUIRED libwnck-3.0)
pkg_check_modules(QTGCONF REQUIRED libqtgconf)
pkg_check_modules(NUXCORE REQUIRED nux-core-1.0)
pkg_check_modules(PANGO REQUIRED pango)


# GSettings schemas
pkg_check_modules(GLIB REQUIRED glib-2.0)
set (UNITY_2D_SCHEMAS "com.canonical.Unity2d.gschema.xml")
set (UNITY_2D_GCONF_CONVERT "unity-2d.convert")
set (GSETTINGS_DIR "${CMAKE_INSTALL_PREFIX}/share/glib-2.0/schemas/")
set (GCONF_CONVERT_DIR "${CMAKE_INSTALL_PREFIX}/share/GConf/gsettings")
execute_process (COMMAND ${PKG_CONFIG_EXECUTABLE} gio-2.0 --variable glib_compile_schemas  OUTPUT_VARIABLE GLIB_COMPILE_SCHEMAS OUTPUT_STRIP_TRAILING_WHITESPACE)

# Run the schemas validator and error if it fails
execute_process (COMMAND ${GLIB_COMPILE_SCHEMAS} ${UNITY_2D_DATA_DIR} ERROR_VARIABLE _schemas_invalid OUTPUT_STRIP_TRAILING_WHITESPACE)

if (_schemas_invalid)
  message (SEND_ERROR "Schemas validation error: ${_schemas_invalid}")
endif (_schemas_invalid)

# Actually install and recompile the schemas
message (STATUS "GSettings schemas will be installed into ${GSETTINGS_DIR}")
install (FILES ${UNITY_2D_DATA_DIR}/${UNITY_2D_SCHEMAS} DESTINATION ${GSETTINGS_DIR})
install (CODE "message (STATUS \"Compiling GSettings schemas\")")
install (CODE "execute_process (COMMAND ${GLIB_COMPILE_SCHEMAS} ${GSETTINGS_DIR})")

# Install GConf to GSettings conversion file
install (FILES ${UNITY_2D_DATA_DIR}/${UNITY_2D_GCONF_CONVERT} DESTINATION ${GCONF_CONVERT_DIR})

include_directories(
    ${CMAKE_BINARY_DIR}
    ${QT_INCLUDE_DIR}
    ${QT_QTCORE_INCLUDE_DIR}
    ${QT_QTDBUS_INCLUDE_DIR}
    ${QT_QTDECLARATIVE_INCLUDE_DIR}
    ${QT_QTXML_INCLUDE_DIR}
    ${QT_QTGUI_INCLUDE_DIR}
    ${QT_QTOPENGL_INCLUDE_DIR}
    ${X11_INCLUDE_DIR}
    )

# Tests
add_custom_target(check)

# Source
add_subdirectory(libunity-2d-private)
add_subdirectory(launcher)
add_subdirectory(panel)
add_subdirectory(places)
add_subdirectory(spread)
add_subdirectory(po)

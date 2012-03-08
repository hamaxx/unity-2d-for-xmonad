project(unity-2d)
cmake_minimum_required(VERSION 2.8)

# Standard install paths
include(GNUInstallDirs)

# Dirs
set(UNITY_2D_DIR "${CMAKE_INSTALL_DATADIR}/unity-2d")
set(UNITY_DIR /usr/share/unity/)
set(UNITY_2D_DATA_DIR "${CMAKE_SOURCE_DIR}/data")

# Configure debian files
string(SUBSTRING "${CMAKE_INSTALL_PREFIX}" 1 -1 DEBIAN_INSTALL_PREFIX)
file(GLOB DEBIAN_FILES debian/*.in)
foreach(in_file ${DEBIAN_FILES})
    string(LENGTH ${in_file} len)
    math(EXPR len "${len} - 3")
    string(SUBSTRING "${in_file}" 0 ${len} file)
    configure_file(${in_file} ${file} @ONLY)
endforeach(in_file)

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
pkg_check_modules(NUXCORE REQUIRED nux-core-2.0)
pkg_check_modules(PANGO REQUIRED pango)
pkg_check_modules(DCONFQT REQUIRED dconf-qt)

# X11_XTest_FOUND is defined by find_package(X11 REQUIRED)
if (NOT X11_XTest_FOUND)
    message(SEND_ERROR "Xtest library not found")
endif (NOT X11_XTest_FOUND)

# GSettings schemas
pkg_check_modules(GLIB REQUIRED glib-2.0)
set (UNITY_2D_SCHEMAS "com.canonical.Unity2d.gschema.xml")
set (UNITY_2D_GCONF_CONVERT "unity-2d.convert")
set (GSETTINGS_DIR "${CMAKE_INSTALL_PREFIX}/share/glib-2.0/schemas/")
set (UNITY_2D_SCHEMA_FILE "${GSETTINGS_DIR}${UNITY_2D_SCHEMAS}")
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

configure_file(config.h.in ${CMAKE_CURRENT_BINARY_DIR}/config.h @ONLY)

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
    ${DCONFQT_INCLUDE_DIRS}
    )

# Tests
enable_testing()
add_custom_target(check make test CTEST_OUTPUT_ON_FAILURE=1)
configure_file(tests/misc/binary_dir.txt.in ${CMAKE_CURRENT_SOURCE_DIR}/tests/misc/binary_dir.txt @ONLY)

# Source
add_subdirectory(libunity-2d-private)
add_subdirectory(panel)
add_subdirectory(spread)
add_subdirectory(shell)
add_subdirectory(po)
add_subdirectory(tests/getshape)

# uninstall target
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake" IMMEDIATE @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

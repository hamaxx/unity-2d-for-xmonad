project(panelplugin-appname)

# Dependencies
pkg_check_modules(QTBAMF REQUIRED libqtbamf)
pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)

# Sources
set(appname_SRCS
    appnameapplet.cpp
    menubarwidget.cpp
    registrar.cpp
    windowhelper.cpp
    plugin.cpp
    )

qt4_add_dbus_adaptor(appname_SRCS com.canonical.AppMenu.Registrar.xml
    registrar.h Registrar
    )

# Build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${QTBAMF_INCLUDE_DIRS}
    ${DBUSMENUQT_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    ${WNCK_INCLUDE_DIRS}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

qt4_automoc(${appname_SRCS})
add_library(panelplugin-appname SHARED ${appname_SRCS})

set_target_properties(panelplugin-appname PROPERTIES LIBRARY_OUTPUT_DIRECTORY "..")

target_link_libraries(panelplugin-appname
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${GTK_LIBRARIES}
    ${WNCK_LIBRARIES}
    ${DBUSMENUQT_LDFLAGS}
    unity-2d-private
    uqapplets
    )


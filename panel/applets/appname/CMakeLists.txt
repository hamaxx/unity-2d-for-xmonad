project(panelplugin-appname)

# Dependencies
pkg_check_modules(QTBAMF REQUIRED libqtbamf)
pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)

# Sources
set(appname_SRCS
    appnameapplet.cpp
    croppedlabel.cpp
    menubarwidget.cpp
    windowhelper.cpp
    plugin.cpp
    )

# Build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${DBUSMENUQT_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    ${QTBAMF_INCLUDE_DIRS}
    ${QTGCONF_INCLUDE_DIRS}
    ${UNITYCORE_INCLUDE_DIRS}
    ${WNCK_INCLUDE_DIRS}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

qt4_automoc(${appname_SRCS})
add_library(panelplugin-appname SHARED ${appname_SRCS})

set_target_properties(panelplugin-appname PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ".."
    VERSION 0
    SOVERSION 0.0
    )

target_link_libraries(panelplugin-appname
    ${DBUSMENUQT_LDFLAGS}
    ${GTK_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${QTGCONF_LDFLAGS}
    ${UNITYCORE_LDFLAGS}
    ${WNCK_LIBRARIES}
    unity-2d-private
    )

install(TARGETS panelplugin-appname
        LIBRARY DESTINATION lib/unity-2d/plugins/panel
       )

project(uqapplets)

# Dependencies
include(FindPkgConfig)

pkg_check_modules(QTBAMF REQUIRED libqtbamf)
pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)

# Sources
set(uqapplets_SRCS
    appindicator/appindicatorapplet.cpp
    appindicator/sniitem.cpp
    appname/appnameapplet.cpp
    appname/croppedlabel.cpp
    appname/menubarwidget.cpp
    appname/windowhelper.cpp
    common/applet.cpp
    common/indicatorentrywidget.cpp
    common/indicatorsmanager.cpp
    common/indicatorswidget.cpp
    common/cairoutils.cpp
    common/panelstyle.cpp
    homebutton/homebuttonapplet.cpp
    homebutton/homebutton.cpp
    indicator/indicatorapplet.cpp
    legacytray/legacytrayapplet.cpp
    legacytray/fdoselectionmanager.cpp
    legacytray/fdotask.cpp
    legacytray/x11embedcontainer.cpp
    legacytray/x11embeddelegate.cpp
    legacytray/x11embedpainter.cpp
    )

qt4_automoc(${uqapplets_SRCS})

# Build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/appindicator
    ${CMAKE_CURRENT_SOURCE_DIR}/appname
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/homebutton
    ${CMAKE_CURRENT_SOURCE_DIR}/indicator
    ${CMAKE_CURRENT_SOURCE_DIR}/unitycore
    ${QTBAMF_INCLUDE_DIRS}
    ${QTGCONF_INCLUDE_DIRS}
    ${DBUSMENUQT_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    ${WNCK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${X11_INCLUDE_DIR}
    ${UNITYCORE_INCLUDE_DIRS}
    ${NUXCORE_INCLUDE_DIRS}
    ${PANGO_INCLUDE_DIRS}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

add_library(uqapplets ${uqapplets_SRCS})

target_link_libraries(uqapplets
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${DBUSMENUQT_LDFLAGS}
    ${QTBAMF_LDFLAGS}
    ${QTGCONF_LDFLAGS}
    ${GTK_LDFLAGS}
    ${WNCK_LDFLAGS}
    ${X11_LIBRARIES}
    ${X11_Xrender_LIB}
    ${X11_Xcomposite_LIB}
    ${X11_Xdamage_LIB}
    ${X11_Xfixes_LIB}
    ${UNITYCORE_LDFLAGS}
    ${NUXCORE_LDFLAGS}
    ${PANGO_LDFLAGS}
    unity-2d-private
    )

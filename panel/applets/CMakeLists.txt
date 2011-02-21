project(uqapplets)

# Dependencies
include(FindPkgConfig)

pkg_check_modules(QTBAMF REQUIRED libqtbamf)
pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)
pkg_check_modules(GTK REQUIRED gtk+-2.0)
pkg_check_modules(INDICATOR REQUIRED indicator)
pkg_check_modules(WNCK REQUIRED libwnck)

find_package(X11 REQUIRED)

# Sources
set(uqapplets_SRCS
    appindicator/appindicatorapplet.cpp
    appindicator/sniitem.cpp
    appname/appnameapplet.cpp
    appname/menubarwidget.cpp
    appname/registrar.cpp
    appname/windowhelper.cpp
    common/applet.cpp
    homebutton/homebuttonapplet.cpp
    indicator/abstractindicator.cpp
    indicator/datetimeindicator.cpp
    indicator/indicatorapplet.cpp
    indicator/indicatorservicemanager.cpp
    indicator/indicator.c
    legacytray/legacytrayapplet.cpp
    legacytray/fdoselectionmanager.cpp
    legacytray/fdotask.cpp
    legacytray/x11embedcontainer.cpp
    legacytray/x11embeddelegate.cpp
    legacytray/x11embedpainter.cpp
    )

qt4_add_dbus_adaptor(uqapplets_SRCS appname/com.canonical.AppMenu.Registrar.xml
    registrar.h Registrar
    )

qt4_automoc(${uqapplets_SRCS})

# Build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/appindicator
    ${CMAKE_CURRENT_SOURCE_DIR}/appname
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/homebutton
    ${CMAKE_CURRENT_SOURCE_DIR}/indicator
    ${QTBAMF_INCLUDE_DIRS}
    ${DBUSMENUQT_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    ${INDICATOR_INCLUDE_DIRS}
    ${WNCK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${X11_INCLUDE_DIR}
    )

add_library(uqapplets ${uqapplets_SRCS})

target_link_libraries(uqapplets
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${DBUSMENUQT_LDFLAGS}
    ${QTBAMF_LDFLAGS}
    ${GTK_LDFLAGS}
    ${INDICATOR_LDFLAGS}
    ${WNCK_LDFLAGS}
    ${X11_LIBRARIES}
    ${X11_Xrender_LIB}
    ${X11_Xcomposite_LIB}
    ${X11_Xdamage_LIB}
    ${X11_Xfixes_LIB}
    )

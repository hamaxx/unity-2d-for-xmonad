project(uqapplets)

macro(read_pkg_variable cmake_var pkg pkg_var)
    execute_process(
        COMMAND pkg-config --variable=${pkg_var} ${pkg}
        OUTPUT_VARIABLE tmp
        )
    # Remove trailing newline from ${tmp}
    string(STRIP "${tmp}" ${cmake_var})
endmacro(read_pkg_variable)

# Dependencies
include(FindPkgConfig)

pkg_check_modules(QTBAMF REQUIRED libqtbamf)
pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)
pkg_check_modules(INDICATOR REQUIRED indicator)

# Get indicator dirs from pkgconfig
read_pkg_variable(INDICATOR_DIR       indicator indicatordir)
read_pkg_variable(INDICATOR_ICONS_DIR indicator iconsdir)
configure_file(indicator-config.h.in indicator-config.h)

# Sources
set(uqapplets_SRCS
    appindicator/appindicatorapplet.cpp
    appindicator/sniitem.cpp
    appname/appnameapplet.cpp
    appname/menubarwidget.cpp
    appname/registrar.cpp
    appname/windowhelper.cpp
    common/applet.cpp
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

# Build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/appindicator
    ${CMAKE_CURRENT_SOURCE_DIR}/appname
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_SOURCE_DIR}/indicator
    ${QTBAMF_INCLUDE_DIRS}
    ${DBUSMENUQT_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    ${INDICATOR_INCLUDE_DIRS}
    ${WNCK_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${X11_INCLUDE_DIR}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

qt4_automoc(${uqapplets_SRCS})
add_library(uqapplets ${uqapplets_SRCS})
SET_TARGET_PROPERTIES(uqapplets PROPERTIES COMPILE_FLAGS -fPIC)

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
    unity-2d-private
    )

add_subdirectory(homebutton)

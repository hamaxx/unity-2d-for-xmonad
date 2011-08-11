project(uqapplets)

# Dependencies
include(FindPkgConfig)

#pkg_check_modules(QTBAMF REQUIRED libqtbamf)
#pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)
#pkg_check_modules(INDICATOR REQUIRED indicator)

# Sources
set(uqapplets_SRCS
    common/applet.cpp
    )

# Build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/common
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

# This is necessary otherwise the plugins (which are shared libraries)
# can't statically link to it. The alternative is to make uqapplets shared too.
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
add_subdirectory(appname)
add_subdirectory(legacytray)
add_subdirectory(indicator)
add_subdirectory(appindicator)

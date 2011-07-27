# Dependencies
pkg_check_modules(GEIS REQUIRED libutouch-geis)
pkg_check_modules(QTGCONF REQUIRED libqtgconf)
pkg_check_modules(DCONFQT REQUIRED dconf-qt)

# Sources
set(launcher_SRCS
    visibilitycontroller.cpp
    launcherview.cpp
    launcherdbus.cpp
    gesturehandler.cpp
    )

set(launcher_MOC_HDRS
    visibilitycontroller.h
    launcherview.h
    launcherdbus.h
    gesturehandler.h
    )

qt4_wrap_cpp(launcher_MOC_SRCS ${launcher_MOC_HDRS})

configure_file(unity-2d-launcher.service.in unity-2d-launcher.service)

qt4_add_dbus_adaptor(launcher_SRCS launcher.xml
    launcherdbus.h LauncherDBus
    )

# Build
add_library(uqlauncher ${launcher_SRCS} ${launcher_MOC_SRCS})
add_executable(unity-2d-launcher launcher.cpp)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GTK_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    ${GEIS_INCLUDE_DIRS}
    ${QTGCONF_INCLUDE_DIRS}
    ${DCONFQT_INCLUDE_DIRS}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

target_link_libraries(uqlauncher
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTDBUS_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${GTK_LDFLAGS}
    ${X11_LDFLAGS}
    ${GEIS_LDFLAGS}
    ${QTGCONF_LDFLAGS}
    ${DCONFQT_LDFLAGS}
    unity-2d-private
    )

target_link_libraries(unity-2d-launcher
    uqlauncher)

# Install
install(TARGETS unity-2d-launcher
    RUNTIME DESTINATION bin
    )

install(FILES unity-2d-launcher.desktop
    DESTINATION share/applications
    )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/unity-2d-launcher.service
    DESTINATION share/dbus-1/services
    )


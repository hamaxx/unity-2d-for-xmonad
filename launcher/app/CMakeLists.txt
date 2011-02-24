# Dependencies
pkg_check_modules(GTK REQUIRED gtk+-2.0)
pkg_check_modules(X11 REQUIRED x11)
pkg_check_modules(WNCK REQUIRED libwnck)
pkg_check_modules(QTGCONF REQUIRED libqtgconf)
pkg_check_modules(GEIS REQUIRED libutouch-geis)

# Sources
set(launcher_SRCS
    autohidecontroller.cpp
    hidemodecontroller.cpp
    intellihidecontroller.cpp
    launcherview.cpp
    launchercontrol.cpp
    gesturehandler.cpp
    )

set(launcher_MOC_HDRS
    autohidecontroller.h
    hidemodecontroller.h
    intellihidecontroller.h
    launcherview.h
    launchercontrol.h
    gesturehandler.h
    )

qt4_wrap_cpp(launcher_MOC_SRCS ${launcher_MOC_HDRS})

configure_file(unity-2d-launcher.service.in unity-2d-launcher.service)

qt4_add_dbus_adaptor(launcher_SRCS launcher.xml
    launchercontrol.h LauncherControl
    )

# Build
add_library(uqlauncher ${launcher_SRCS} ${launcher_MOC_SRCS})
add_executable(unity-2d-launcher launcher.cpp)

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GTK_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    ${WNCK_INCLUDE_DIRS}
    ${QTGCONF_INCLUDE_DIRS}
    ${GEIS_INCLUDE_DIRS}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

target_link_libraries(uqlauncher
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTDBUS_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${GTK_LDFLAGS}
    ${X11_LDFLAGS}
    ${WNCK_LDFLAGS}
    ${QTGCONF_LDFLAGS}
    ${GEIS_LDFLAGS}
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


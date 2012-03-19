# Dependencies
pkg_check_modules(QTGCONF REQUIRED libqtgconf)
pkg_check_modules(DCONFQT REQUIRED dconf-qt)

# Sources
set(shell_SRCS
    shell.cpp
    shelldbus.cpp
    dashdbus.cpp
    huddbus.cpp
    shelldeclarativeview.cpp
    shellmanager.cpp
    )

set(shell_MOC_HDRS
    shelldbus.h
    dashdbus.h
    huddbus.h
    shelldeclarativeview.h
    shellmanager.h
    )

qt4_wrap_cpp(shell_MOC_SRCS ${shell_MOC_HDRS})

# Build
configure_file(unity-2d-places.service.in unity-2d-places.service)
qt4_add_dbus_adaptor(shell_SRCS dash.xml dashdbus.h DashDBus)
qt4_add_dbus_adaptor(shell_SRCS hud.xml huddbus.h HUDDBus)

add_executable(unity-2d-shell ${shell_SRCS} ${shell_MOC_SRCS})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QTGCONF_INCLUDE_DIRS}
    ${DCONFQT_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    ${QTBAMF_INCLUDE_DIRS}
    ${WNCK_INCLUDE_DIRS}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

target_link_libraries(unity-2d-shell
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTDBUS_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${QTGCONF_LDFLAGS}
    ${DCONFQT_LDFLAGS}
    ${GTK_LDFLAGS}
    ${WNCK_LDFLAGS}
    ${QTBAMF_LDFLAGS}
    ${X11_Xext_LIB}
    ${X11_X11_LIB}
    unity-2d-private
    )

# Install
install(TARGETS unity-2d-shell
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/unity-2d-places.service
    DESTINATION ${CMAKE_INSTALL_DATADIR}/dbus-1/services
    )

install(FILES unity-2d-shell.desktop
    DESTINATION ${CMAKE_INSTALL_DATADIR}/applications
    )

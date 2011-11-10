# Dependencies
pkg_check_modules(QTGCONF REQUIRED libqtgconf)

# Sources
set(shell_SRCS
    shell.cpp
    dashdbus.cpp
    shelldeclarativeview.cpp
    )

set(shell_MOC_HDRS
    dashdbus.h
    shelldeclarativeview.h
    )

qt4_wrap_cpp(shell_MOC_SRCS ${shell_MOC_HDRS})

# Build
configure_file(unity-2d-places.service.in unity-2d-places.service)

qt4_add_dbus_adaptor(shell_SRCS dash.xml dashdbus.h DashDBus)

add_executable(unity-2d-shell ${shell_SRCS} ${shell_MOC_SRCS})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QTGCONF_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

target_link_libraries(unity-2d-shell
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTDBUS_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${QTGCONF_LDFLAGS}
    ${GTK_LDFLAGS}
    ${X11_Xext_LIB}
    ${X11_X11_LIB}
    unity-2d-private
    )

# Install
install(TARGETS unity-2d-shell
    RUNTIME DESTINATION bin
    )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/unity-2d-places.service
    DESTINATION share/dbus-1/services
    )

install(FILES unity-2d-shell.desktop
    DESTINATION share/applications
    )

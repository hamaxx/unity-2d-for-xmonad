# Dependencies
pkg_check_modules(QTGCONF REQUIRED libqtgconf)

# Sources
set(places_SRCS
    places.cpp
    dashdeclarativeview.cpp
    )

set(places_MOC_HDRS
    dashdeclarativeview.h
    )

qt4_wrap_cpp(places_MOC_SRCS ${places_MOC_HDRS})

# Build
configure_file(unity-2d-places.service.in unity-2d-places.service)

qt4_add_dbus_adaptor(places_SRCS dash.xml
    dashdeclarativeview.h DashDeclarativeView
    )

add_executable(unity-2d-places ${places_SRCS} ${places_MOC_SRCS})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QTGCONF_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

target_link_libraries(unity-2d-places
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
install(TARGETS unity-2d-places
    RUNTIME DESTINATION bin
    )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/unity-2d-places.service
    DESTINATION share/dbus-1/services
    )

install(FILES unity-2d-places.desktop
    DESTINATION share/applications
    )

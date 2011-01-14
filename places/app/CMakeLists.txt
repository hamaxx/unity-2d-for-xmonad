# Dependencies
find_package(X11 REQUIRED)

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
configure_file(unity-2d-places.service.in unity-qt-places.service)

add_executable(unity-2d-places ${places_SRCS} ${places_MOC_SRCS})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    )

target_link_libraries(unity-2d-places
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTDBUS_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${X11_Xext_LIB}
    ${X11_X11_LIB}
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

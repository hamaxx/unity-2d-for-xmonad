# Sources
set(spread_SRCS
    spread.cpp
    spreadview.cpp
    spreadcontrol.cpp
   )

set(spread_MOC_HDRS
    spreadview.h
    spreadcontrol.h
    )

qt4_wrap_cpp(spread_MOC_SRCS ${spread_MOC_HDRS})

configure_file(unity-2d-spread.service.in unity-2d-spread.service)

qt4_add_dbus_adaptor(spread_SRCS spread.xml
    spreadcontrol.h SpreadControl
    )

# Build
add_executable(unity-2d-spread ${spread_SRCS} ${spread_MOC_SRCS})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GTK_INCLUDE_DIRS}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

target_link_libraries(unity-2d-spread
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTDBUS_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${GTK_LDFLAGS}
    ${X11_Xext_LIB}
    ${X11_X11_LIB}
    unity-2d-private
    )

# Install
install(TARGETS unity-2d-spread
    RUNTIME DESTINATION bin
    )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/unity-2d-spread.service
    DESTINATION share/dbus-1/services
    )


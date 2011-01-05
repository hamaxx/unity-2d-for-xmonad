# Dependencies
pkg_check_modules(GTK REQUIRED gtk+-2.0)

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

configure_file(unity-qt-spread.service.in unity-qt-spread.service)

qt4_add_dbus_adaptor(spread_SRCS spread.xml
    spreadcontrol.h SpreadControl
    )

# Build
add_executable(unity-qt-spread ${spread_SRCS} ${spread_MOC_SRCS})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GTK_INCLUDE_DIRS}
    )

target_link_libraries(unity-qt-spread
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTDBUS_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${GTK_LDFLAGS}
    )

# Install
install(TARGETS unity-qt-spread
    RUNTIME DESTINATION bin
    )

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/unity-qt-spread.service
    DESTINATION share/dbus-1/services
    )

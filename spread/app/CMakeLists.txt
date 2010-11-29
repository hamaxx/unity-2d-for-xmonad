# Dependencies
pkg_check_modules(GTK REQUIRED gtk+-2.0)
pkg_check_modules(X11 REQUIRED x11)

# Sources
set(spread_SRCS
    spread.cpp
    spreadview.cpp
    )

set(spread_MOC_HDRS
    spreadview.h
    )

qt4_wrap_cpp(spread_MOC_SRCS ${spread_MOC_HDRS})

# Build
add_executable(unity-qt-spread ${spread_SRCS} ${spread_MOC_SRCS})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GTK_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    )

target_link_libraries(unity-qt-spread
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${GTK_LDFLAGS}
    ${X11_LDFLAGS}
    )

# Install
install(TARGETS unity-qt-spread
    RUNTIME DESTINATION bin
    )

install(FILES unity-qt-spread.desktop
    DESTINATION share/applications
    )

# Dependencies
pkg_check_modules(GTK REQUIRED gtk+-2.0)
pkg_check_modules(X11 REQUIRED x11)

# Sources
set(launcher_SRCS
    launcher.cpp
    launcherview.cpp
    draganddrophelper.cpp
    )

set(launcher_MOC_HDRS
    launcherview.h
    draganddrophelper.h
    )

qt4_wrap_cpp(launcher_MOC_SRCS ${launcher_MOC_HDRS})

# Build
add_executable(unity-qt-launcher ${launcher_SRCS} ${launcher_MOC_SRCS})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GTK_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    )

target_link_libraries(unity-qt-launcher
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${GTK_LDFLAGS}
    ${X11_LDFLAGS}
    )

# Install
install(TARGETS unity-qt-launcher
    RUNTIME DESTINATION bin
    )

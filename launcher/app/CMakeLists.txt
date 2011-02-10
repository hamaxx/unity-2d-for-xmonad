# Dependencies
pkg_check_modules(GTK REQUIRED gtk+-2.0)
pkg_check_modules(X11 REQUIRED x11)

# Sources
set(launcher_SRCS
    launcherview.cpp
    )

set(launcher_MOC_HDRS
    launcherview.h
    )

qt4_wrap_cpp(launcher_MOC_SRCS ${launcher_MOC_HDRS})

# Build
add_library(uqlauncher ${launcher_SRCS} ${launcher_MOC_SRCS})
add_executable(unity-2d-launcher launcher.cpp)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${GTK_INCLUDE_DIRS}
    ${X11_INCLUDE_DIRS}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

target_link_libraries(uqlauncher
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${GTK_LDFLAGS}
    ${X11_LDFLAGS}
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

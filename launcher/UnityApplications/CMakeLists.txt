# Dependencies
pkg_check_modules(QTBAMF REQUIRED libqtbamf)
pkg_check_modules(QTGCONF REQUIRED libqtgconf)
pkg_check_modules(GDK REQUIRED gdk-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(WNCK REQUIRED libwnck-1.0)

# Sources
set(UnityApplications_SRCS
    iconimageprovider.cpp
    launcheritem.cpp
    launcherapplication.cpp
    launcherapplicationslist.cpp
    device.cpp
    deviceslist.cpp
    launchermodel.cpp
    launchermenu.cpp
    plugin.cpp
    )

set(UnityApplications_MOC_HDRS
    launcheritem.h
    launcherapplication.h
    launcherapplicationslist.h
    device.h
    deviceslist.h
    launchermodel.h
    launchermenu.h
    plugin.h
    )

qt4_wrap_cpp(UnityApplications_MOC_SRCS ${UnityApplications_MOC_HDRS})

# Build
add_definitions(-DWNCK_I_KNOW_THIS_IS_UNSTABLE)

add_library(UnityApplications SHARED ${UnityApplications_SRCS} ${UnityApplications_MOC_SRCS})

set_target_properties(UnityApplications PROPERTIES
    SOVERSION 1
    VERSION 1.0.0
    )

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${QTBAMF_INCLUDE_DIRS}
    ${QTGCONF_INCLUDE_DIRS}
    ${GDK_INCLUDE_DIRS}
    ${GIO_INCLUDE_DIRS}
    ${WNCK_INCLUDE_DIRS}
    )

target_link_libraries(UnityApplications
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${QTBAMF_LDFLAGS}
    ${QTGCONF_LDFLAGS}
    ${GDK_LDFLAGS}
    ${GIO_LDFLAGS}
    ${WNCK_LDFLAGS}
    )

# Install
set(IMPORT_INSTALL_DIR lib/qt4/imports/UnityApplications)

install(TARGETS UnityApplications
    LIBRARY DESTINATION ${IMPORT_INSTALL_DIR}
    )

install(FILES qmldir
    DESTINATION ${IMPORT_INSTALL_DIR}
    )

install(FILES launchermenu.css
    DESTINATION ${UNITY_QT_DIR}/launcher
    )


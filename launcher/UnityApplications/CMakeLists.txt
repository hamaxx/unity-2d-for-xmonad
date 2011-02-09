# Dependencies
pkg_check_modules(QTBAMF REQUIRED libqtbamf)
pkg_check_modules(QTGCONF REQUIRED libqtgconf)
pkg_check_modules(QTDEE REQUIRED libqtdee)
pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)
pkg_check_modules(GDK REQUIRED gdk-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(WNCK REQUIRED libwnck-1.0)

# Sources
set(UnityApplications_SRCS
    iconimageprovider.cpp
    listaggregatormodel.cpp
    launcheritem.cpp
    launcherapplication.cpp
    launcherapplicationslist.cpp
    launcherdevice.cpp
    launcherdeviceslist.cpp
    placeentry.cpp
    place.cpp
    launcherplaceslist.cpp
    trash.cpp
    launchermenu.cpp
    plugin.cpp
    workspaces.cpp
    )

set(UnityApplications_MOC_HDRS
    listaggregatormodel.h
    launcheritem.h
    launcherapplication.h
    launcherapplicationslist.h
    launcherdevice.h
    launcherdeviceslist.h
    placeentry.h
    place.h
    launcherplaceslist.h
    trash.h
    launchermenu.h
    plugin.h
    workspaces.h
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
    ${QTDEE_INCLUDE_DIRS}
    ${DBUSMENUQT_INCLUDE_DIRS}
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
    ${QTDEE_LDFLAGS}
    ${DBUSMENUQT_LDFLAGS}
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


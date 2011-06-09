# Dependencies
pkg_check_modules(WNCK REQUIRED libwnck-1.0)
pkg_check_modules(QTBAMF REQUIRED libqtbamf)
pkg_check_modules(QTGCONF REQUIRED libqtgconf)
pkg_check_modules(QTDEE REQUIRED libqtdee)
pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)
pkg_check_modules(GLIB REQUIRED glib-2.0)
pkg_check_modules(GDK REQUIRED gdk-2.0)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(STARTUPNOTIFICATION REQUIRED libstartup-notification-1.0)
pkg_check_modules(INDICATOR REQUIRED indicator)

# Sources
set(unity-2d-private-qml_SRCS
    plugin.cpp
    qsortfilterproxymodelqml.cpp
    blendedimageprovider.cpp
    windowimageprovider.cpp
    windowinfo.cpp
    windowslist.cpp
    screeninfo.cpp
    cacheeffect.cpp
    workspacesinfo.cpp
    signalwaiter.cpp
    dragitem.cpp
    dragitemwithurl.cpp
    dropitem.cpp
    iconimageprovider.cpp
    listaggregatormodel.cpp
    launcheritem.cpp
    launcherapplication.cpp
    launcherapplicationslist.cpp
    launcherapplicationslistdbus.cpp
    launcherdevice.cpp
    launcherdeviceslist.cpp
    launcherutility.cpp
    placeentry.cpp
    place.cpp
    launcherplaceslist.cpp
    trash.cpp
    launchermenu.cpp
    webfavorite.cpp
    workspaces.cpp
    launcherdropitem.cpp
    )

set(unity-2d-private-qml_MOC_HDRS
    plugin.h
    qsortfilterproxymodelqml.h
    windowinfo.h
    windowslist.h
    screeninfo.h
    cacheeffect.h
    workspacesinfo.h
    signalwaiter.h
    dragitem.h
    dragitemwithurl.h
    dropitem.h
    listaggregatormodel.h
    launcheritem.h
    launcherapplication.h
    launcherapplicationslist.h
    launcherapplicationslistdbus.h
    launcherdevice.h
    launcherdeviceslist.h
    launcherutility.h
    placeentry.h
    place.h
    launcherplaceslist.h
    trash.h
    launchermenu.h
    webfavorite.h
    workspaces.h
    launcherdropitem.h
    )

file(GLOB unity-2d-private-qml_QML *.qml)

qt4_wrap_cpp(unity-2d-private-qml_MOC_SRCS ${unity-2d-private-qml_MOC_HDRS})

# Build
add_library(unity-2d-private-qml SHARED ${unity-2d-private-qml_SRCS} ${unity-2d-private-qml_MOC_SRCS})
add_definitions(-DWNCK_I_KNOW_THIS_IS_UNSTABLE -DSN_API_NOT_YET_FROZEN)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${WNCK_INCLUDE_DIRS}
    ${QTBAMF_INCLUDE_DIRS}
    ${QTGCONF_INCLUDE_DIRS}
    ${QTDEE_INCLUDE_DIRS}
    ${DBUSMENUQT_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${GDK_INCLUDE_DIRS}
    ${GIO_INCLUDE_DIRS}
    ${STARTUPNOTIFICATION_INCLUDE_DIRS}
    ${INDICATOR_INCLUDE_DIRS}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

target_link_libraries(unity-2d-private-qml
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTDBUS_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTNETWORK_LIBRARIES}
    ${WNCK_LDFLAGS}
    ${QTBAMF_LDFLAGS}
    ${QTGCONF_LDFLAGS}
    ${QTDEE_LDFLAGS}
    ${DBUSMENUQT_LDFLAGS}
    ${GLIB_LDFLAGS}
    ${GDK_LDFLAGS}
    ${GIO_LDFLAGS}
    ${STARTUPNOTIFICATION_LDFLAGS}
    ${INDICATOR_LDFLAGS}
    unity-2d-private
    )

# Install
set(IMPORT_INSTALL_DIR lib/qt4/imports/Unity2d)

install(TARGETS unity-2d-private-qml
    LIBRARY DESTINATION ${IMPORT_INSTALL_DIR}
    )

install(FILES qmldir ${unity-2d-private-qml_QML}
    DESTINATION ${IMPORT_INSTALL_DIR}
    )


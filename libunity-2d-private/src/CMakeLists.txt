set (LIB_NAME "unity-2d-private")

# Sources
set(lib${LIB_NAME}_SRCS
    bfb.cpp
    dashclient.cpp
    debug.cpp
    gconnector.cpp
    gimageutils.cpp
    gnomesessionclient.cpp
    keyboardmodifiersmonitor.cpp
    hotkeymonitor.cpp
    hotkey.cpp
    keymonitor.cpp
    launcherclient.cpp
    mousearea.cpp
    unity2dapplication.cpp
    unity2ddebug.cpp
    unity2dpanel.cpp
    unity2dtr.cpp
    unity2ddeclarativeview.cpp
    mimedata.cpp
    dragdropevent.cpp
    propertybinder.cpp
    abstractvisibilitybehavior.cpp
    autohidebehavior.cpp
    edgehitdetector.cpp
    forcevisiblebehavior.cpp
    intellihidebehavior.cpp
    giodefaultapplication.cpp
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
    iconutilities.cpp
    lenses.cpp
    lens.cpp
    filter.cpp
    filteroption.cpp
    ratingsfilter.cpp
    radiooptionfilter.cpp
    checkoptionfilter.cpp
    multirangefilter.cpp
    filters.cpp
    cairoutils.cpp
    indicatorentrywidget.cpp
    indicatorsmanager.cpp
    indicatorswidget.cpp
    panelapplet.cpp
    panelstyle.cpp
    percentcoder.cpp
    )

# Build
qt4_automoc(${libunity-2d-private_SRCS})

add_definitions(-DWNCK_I_KNOW_THIS_IS_UNSTABLE -DSN_API_NOT_YET_FROZEN)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLIB_INCLUDE_DIRS}
    ${GTK_INCLUDE_DIRS}
    ${PANGO_INCLUDE_DIRS}
    ${WNCK_INCLUDE_DIRS}
    ${QTBAMF_INCLUDE_DIRS}
    ${QTGCONF_INCLUDE_DIRS}
    ${QTDEE_INCLUDE_DIRS}
    ${DBUSMENUQT_INCLUDE_DIRS}
    ${GDK_INCLUDE_DIRS}
    ${GIO_INCLUDE_DIRS}
    ${STARTUPNOTIFICATION_INCLUDE_DIRS}
    ${INDICATOR_INCLUDE_DIRS}
    ${X11_INCLUDE_DIR}
    ${DCONFQT_INCLUDE_DIRS}
    ${UNITYCORE_INCLUDE_DIRS}
    ${NUXCORE_INCLUDE_DIRS}
    ${DEE_INCLUDE_DIRS}
    ${XINPUT_INCLUDE_DIRS}
    )

add_library(${LIB_NAME} SHARED ${libunity-2d-private_SRCS} listmodelwrapper.h)
set_target_properties(${LIB_NAME} PROPERTIES
    VERSION ${libunity-2d-private_VERSION}
    SOVERSION ${libunity-2d-private_SOVERSION}
    )

target_link_libraries(${LIB_NAME}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTDBUS_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${QT_QTOPENGL_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTNETWORK_LIBRARIES}
    ${X11_LIBRARIES}
    ${GLIB_LDFLAGS}
    ${GTK_LDFLAGS}
    ${PANGO_LDFLAGS}
    ${WNCK_LDFLAGS}
    ${GDK_LDFLAGS}
    ${GIO_LDFLAGS}
    ${X11_Xcomposite_LIB}
    ${QTBAMF_LDFLAGS}
    ${QTGCONF_LDFLAGS}
    ${QTDEE_LDFLAGS}
    ${DBUSMENUQT_LDFLAGS}
    ${STARTUPNOTIFICATION_LDFLAGS}
    ${INDICATOR_LDFLAGS}
    ${DCONFQT_LDFLAGS}
    ${UNITYCORE_LDFLAGS}
    ${NUXCORE_LDFLAGS}
    ${DEE_LDFLAGS}
    ${XINPUT_LDFLAGS}
    )

# Install
install(TARGETS ${LIB_NAME}
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    )

#
# PkgConfig file
#
set (PREFIXDIR "${CMAKE_INSTALL_PREFIX}")
set (EXEC_PREFIX "${CMAKE_INSTALL_PREFIX}")
set (LIBDIR "${CMAKE_INSTALL_PREFIX}/lib")
set (INCLUDEDIR "${CMAKE_INSTALL_PREFIX}/include")

install(DIRECTORY .
    DESTINATION include/${LIB_NAME}
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*_p.h" EXCLUDE
    )

configure_file (${LIB_NAME}.pc.cmake ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}.pc)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${LIB_NAME}.pc DESTINATION
         ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)


set (LIB_NAME "unity-2d-private")

# Sources
set(lib${LIB_NAME}_SRCS
    gconnector.cpp
    gnomesessionclient.cpp
    keyboardmodifiersmonitor.cpp
    hotkeymonitor.cpp
    hotkey.cpp
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
    )

# Build
qt4_automoc(${libunity-2d-private_SRCS})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GLIB_INCLUDE_DIRS}
    ${WNCK_INCLUDE_DIRS}
    )

add_library(${LIB_NAME} SHARED ${libunity-2d-private_SRCS})
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
    ${X11_LIBRARIES}
    ${GLIB_LDFLAGS}
    ${WNCK_LDFLAGS}
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


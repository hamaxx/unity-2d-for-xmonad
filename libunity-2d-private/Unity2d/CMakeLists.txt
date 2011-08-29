# Sources
set(unity-2d-private-qml_SRCS
    plugin.cpp
    )

set(unity-2d-private-qml_MOC_HDRS
    plugin.h
    )

file(GLOB unity-2d-private-qml_QML *.qml)

qt4_add_resources(unity-2d-private-qml_RC_SRCS launchermenu/launchermenu.qrc)
qt4_wrap_cpp(unity-2d-private-qml_MOC_SRCS ${unity-2d-private-qml_MOC_HDRS})

# Build
add_library(unity-2d-private-qml SHARED ${unity-2d-private-qml_SRCS} ${unity-2d-private-qml_MOC_SRCS} ${unity-2d-private-qml_RC_SRCS})
add_definitions(-DWNCK_I_KNOW_THIS_IS_UNSTABLE)

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
    ${X11_INCLUDE_DIR}
    ${DCONFQT_INCLUDE_DIRS}
    ${UNITYCORE_INCLUDE_DIRS}
    ${NUXCORE_INCLUDE_DIRS}
    ${DEE_INCLUDE_DIRS}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

target_link_libraries(unity-2d-private-qml
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
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

install(FILES warty-final-ubuntu.jpg
    DESTINATION ${UNITY_2D_DIR}
    )


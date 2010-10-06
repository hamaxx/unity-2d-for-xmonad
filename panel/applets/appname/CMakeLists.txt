set(appname_SRCS
    appnameapplet.cpp
    )
qt4_automoc(${appname_SRCS})

include_directories(
    ${QTBAMF_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

add_executable(appname ${appname_SRCS})

target_link_libraries(appname
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${QTBAMF_LDFLAGS}
    unity-qt-gui
    )

install(TARGETS appname
    RUNTIME DESTINATION ${APPLET_INSTALL_DIR}
    )

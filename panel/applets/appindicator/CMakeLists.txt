set(appindicator_SRCS
    appindicatorapplet.cpp
    sniitem.cpp
    )

qt4_automoc(${appindicator_SRCS})

include_directories(
    ${DBUSMENUQT_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

add_executable(appindicator ${appindicator_SRCS})

target_link_libraries(appindicator
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${DBUSMENUQT_LDFLAGS}
    uqpanel
    )

install(TARGETS appindicator
    RUNTIME DESTINATION ${APPLET_INSTALL_DIR}
    )

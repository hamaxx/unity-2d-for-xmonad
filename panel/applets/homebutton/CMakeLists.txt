set(homebutton_SRCS
    homebuttonapplet.cpp
    )
qt4_automoc(${homebutton_SRCS})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    )

add_executable(homebutton ${homebutton_SRCS})

target_link_libraries(homebutton
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTDBUS_LIBRARIES}
    uqpanel
    )

install(TARGETS homebutton
    RUNTIME DESTINATION ${APPLET_INSTALL_DIR}
    )

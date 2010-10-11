set(appmenu_SRCS
    appmenuapplet.cpp
    registrar.cpp
    )

qt4_add_dbus_adaptor(appmenu_SRCS org.ayatana.AppMenu.Registrar.xml
    registrar.h Registrar
    )

qt4_automoc(${appmenu_SRCS})

include_directories(
    ${QTBAMF_INCLUDE_DIRS}
    ${DBUSMENUQT_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

add_executable(appmenu ${appmenu_SRCS})

target_link_libraries(appmenu
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${DBUSMENUQT_LDFLAGS}
    ${QTBAMF_LDFLAGS}
    uqpanel
    )

install(TARGETS appmenu
    RUNTIME DESTINATION ${APPLET_INSTALL_DIR}
    )

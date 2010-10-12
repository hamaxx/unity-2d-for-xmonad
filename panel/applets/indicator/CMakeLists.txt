set(indicator_SRCS
    abstractindicator.cpp
    datetimeindicator.cpp
    indicatorapplet.cpp
    indicatorservicemanager.cpp
    )

qt4_automoc(${indicator_SRCS})

include_directories(
    ${DBUSMENUQT_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

add_executable(indicator ${indicator_SRCS})

target_link_libraries(indicator
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${DBUSMENUQT_LDFLAGS}
    uqpanel
    )

install(TARGETS indicator
    RUNTIME DESTINATION ${APPLET_INSTALL_DIR}
    )

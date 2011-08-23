pkg_check_modules(DCONFQT REQUIRED dconf-qt)

set(panel_SRCS
    main.cpp
    panelmanager.cpp
    )

qt4_automoc(${panel_SRCS})
add_executable(unity-2d-panel ${panel_SRCS})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${GTK_INCLUDE_DIRS}
    ${UNITYCORE_INCLUDE_DIRS}
    ${DCONFQT_INCLUDE_DIRS}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

target_link_libraries(unity-2d-panel
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${GTK_LDFLAGS}
    ${DCONFQT_LIBRARIES}
    unity-2d-private
    )

install(TARGETS unity-2d-panel
    RUNTIME DESTINATION bin
    )

install(FILES unity-2d-panel.desktop
    DESTINATION share/applications
    )

set(panel_SRCS
    main.cpp
    unity2dstyle.cpp
    )

qt4_automoc(${panel_SRCS})
add_executable(unity-2d-panel ${panel_SRCS})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${uqapplets_SOURCE_DIR}
    ${uqapplets_SOURCE_DIR}/common
    ${libunity-2d-private_SOURCE_DIR}/src
    )

target_link_libraries(unity-2d-panel
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    uqapplets
    unity-2d-private
    )

install(TARGETS unity-2d-panel
    RUNTIME DESTINATION bin
    )

install(FILES unity-2d-panel.desktop
    DESTINATION share/applications
    )

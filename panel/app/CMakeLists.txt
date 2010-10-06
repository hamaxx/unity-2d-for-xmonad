set(panel_SRCS
    main.cpp
    )

qt4_automoc(${panel_SRCS})
add_executable(unity-qt-panel ${panel_SRCS})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    )

target_link_libraries(unity-qt-panel
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    unity-qt-gui
    )

install(TARGETS unity-qt-panel
    RUNTIME DESTINATION bin
    )

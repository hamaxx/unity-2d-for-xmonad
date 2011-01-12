# Sources
set(libuqpanel_SRCS
    applet.cpp
    keyboardmodifiersmonitor.cpp
    panel.cpp
    unityqtapplication.cpp
    unityqtstyle.cpp
    )

set(libuqpanel_VERSION 0.0.0)
set(libuqpanel_SOVERSION 0)

# Build
qt4_automoc(${libuqpanel_SRCS})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    )

configure_file(libuqpanel.pc.in ${CMAKE_BINARY_DIR}/libuqpanel.pc @ONLY)

add_library(uqpanel SHARED ${libuqpanel_SRCS})
set_target_properties(uqpanel PROPERTIES
    VERSION ${libuqpanel_VERSION}
    SOVERSION ${libuqpanel_SOVERSION}
    )

target_link_libraries(uqpanel
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    )

# Install
install(TARGETS uqpanel
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    )

install(DIRECTORY .
    DESTINATION include/uqpanel
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*_p.h" EXCLUDE
    )

install(FILES ${CMAKE_BINARY_DIR}/libuqpanel.pc
    DESTINATION lib/pkgconfig
    )

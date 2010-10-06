set(libgui_SRCS
    applet.cpp
    appletcontainer.cpp
    panel.cpp
    )
qt4_automoc(${libgui_SRCS})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    )

add_library(unity-qt-gui SHARED ${libgui_SRCS})
set_target_properties(unity-qt-gui PROPERTIES
    VERSION 0.0.0
    SOVERSION 0
    )

target_link_libraries(unity-qt-gui
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    )

install(TARGETS unity-qt-gui
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    )

install(DIRECTORY .
    DESTINATION include/unity-qt-gui
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*_p.h" EXCLUDE
    )

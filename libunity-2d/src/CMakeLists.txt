# Sources
set(libunity-2d_SRCS
    keyboardmodifiersmonitor.cpp
    unity2dapplication.cpp
    unity2dpanel.cpp
    )

# Build
qt4_automoc(${libunity-2d_SRCS})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

add_library(unity-2d SHARED ${libunity-2d_SRCS})
set_target_properties(unity-2d PROPERTIES
    VERSION ${libunity-2d_VERSION}
    SOVERSION ${libunity-2d_SOVERSION}
    )

target_link_libraries(unity-2d
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${X11_LIBRARIES}
    )

# Install
install(TARGETS unity-2d
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    )

install(DIRECTORY .
    DESTINATION include/unity-2d
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*_p.h" EXCLUDE
    )

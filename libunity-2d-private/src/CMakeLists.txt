# Sources
set(libunity-2d-private_SRCS
    gettexttranslator.cpp
    gnomesessionclient.cpp
    keyboardmodifiersmonitor.cpp
    mousearea.cpp
    unity2dapplication.cpp
    unity2dpanel.cpp
    mimedata.cpp
    dragdropevent.cpp
    )

# Build
qt4_automoc(${libunity-2d-private_SRCS})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    )

add_library(unity-2d-private SHARED ${libunity-2d-private_SRCS})
set_target_properties(unity-2d-private PROPERTIES
    VERSION ${libunity-2d-private_VERSION}
    SOVERSION ${libunity-2d-private_SOVERSION}
    )

target_link_libraries(unity-2d-private
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTDBUS_LIBRARIES}
    ${X11_LIBRARIES}
    )

# Install
install(TARGETS unity-2d-private
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    )

install(DIRECTORY .
    DESTINATION include/unity-2d-private
    FILES_MATCHING PATTERN "*.h"
    PATTERN "*_p.h" EXCLUDE
    )

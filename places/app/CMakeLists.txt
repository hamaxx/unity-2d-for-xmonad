# Sources
set(places_SRCS
    places.cpp
    dashdeclarativeview.cpp
    )

set(places_MOC_HDRS
    dashdeclarativeview.h
    )

qt4_wrap_cpp(places_MOC_SRCS ${places_MOC_HDRS})

# Build
add_executable(unity-qt-places ${places_SRCS} ${places_MOC_SRCS})

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    )

target_link_libraries(unity-qt-places
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    )

# Install
install(TARGETS unity-qt-places
    RUNTIME DESTINATION bin
    )

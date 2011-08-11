project(uqapplets)

# Sources
set(uqapplets_SRCS
    common/applet.cpp
    )

# Build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/common
    ${CMAKE_CURRENT_BINARY_DIR}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

qt4_automoc(${uqapplets_SRCS})
add_library(uqapplets ${uqapplets_SRCS})

# This is necessary otherwise the plugins (which are shared libraries)
# can't statically link to it. The alternative is to make uqapplets shared too.
SET_TARGET_PROPERTIES(uqapplets PROPERTIES COMPILE_FLAGS -fPIC)

target_link_libraries(uqapplets
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    )

add_subdirectory(homebutton)
add_subdirectory(appname)
add_subdirectory(legacytray)
add_subdirectory(indicator)
add_subdirectory(appindicator)

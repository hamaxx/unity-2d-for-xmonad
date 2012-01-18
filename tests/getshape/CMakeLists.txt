# Sources
set(shell_SRCS
    getshape.cpp
    )

qt4_wrap_cpp(shell_MOC_SRCS ${shell_MOC_HDRS})

# Build

add_executable(getshape ${shell_SRCS} ${shell_MOC_SRCS})

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

target_link_libraries(getshape
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTGUI_LIBRARIES}
    ${X11_Xext_LIB}
    ${X11_X11_LIB}
    )

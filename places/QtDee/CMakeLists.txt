cmake_minimum_required(VERSION 2.8)

# Dependencies
include(FindPkgConfig)
find_package(Qt4 REQUIRED)
pkg_check_modules(DEE REQUIRED dee-1.0)

# Sources
set(QtDee_SRCS
    deelistmodel.cpp
    plugin.cpp
    )

set(QtDee_MOC_HDRS
    deelistmodel.h
    plugin.h
    )

qt4_wrap_cpp(QtDee_MOC_SRCS ${QtDee_MOC_HDRS})

# Build
add_library(QtDee SHARED ${QtDee_SRCS} ${QtDee_MOC_SRCS})
add_executable(test test.cpp)

set_target_properties(QtDee PROPERTIES
    SOVERSION 1
    VERSION 1.0.0
    )

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${DEE_INCLUDE_DIRS}
    )

target_link_libraries(QtDee
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTDBUS_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${DEE_LDFLAGS}
    )

target_link_libraries(test
    QtDee
    )

# Install
set(IMPORT_INSTALL_DIR lib/qt4/imports/QtDee)

install(TARGETS QtDee
    LIBRARY DESTINATION ${IMPORT_INSTALL_DIR}
    )

install(FILES qmldir
    DESTINATION ${IMPORT_INSTALL_DIR}
    )


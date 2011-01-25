cmake_minimum_required(VERSION 2.8)
pkg_check_modules(WNCK REQUIRED libwnck-1.0)
pkg_check_modules(QTBAMF REQUIRED libqtbamf)

# Dependencies
include(FindPkgConfig)
find_package(Qt4 REQUIRED)

# Sources
set(UnityPlaces_SRCS
    plugin.cpp
    qdbusconnectionqml.cpp
    qsortfilterproxymodelqml.cpp
    blendedimageprovider.cpp
    windowimageprovider.cpp
    windowinfo.cpp
    windowslist.cpp
    screeninfo.cpp
    cacheeffect.cpp
    )

set(UnityPlaces_MOC_HDRS
    unity_place.h
    unity_place_entry.h
    plugin.h
    qdbusconnectionqml.h
    qsortfilterproxymodelqml.h
    blendedimageprovider.h
    windowimageprovider.h
    windowinfo.h
    windowslist.h
    screeninfo.h
    cacheeffect.h
    )

qt4_wrap_cpp(UnityPlaces_MOC_SRCS ${UnityPlaces_MOC_HDRS})

# Build
add_library(UnityPlaces SHARED ${UnityPlaces_SRCS} ${UnityPlaces_MOC_SRCS})
add_definitions(-DWNCK_I_KNOW_THIS_IS_UNSTABLE)

set_target_properties(UnityPlaces PROPERTIES
    SOVERSION 1
    VERSION 1.0.0
    )

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${WNCK_INCLUDE_DIRS}
    ${QTBAMF_INCLUDE_DIRS}
    )

target_link_libraries(UnityPlaces
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTDBUS_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${WNCK_LDFLAGS}
    ${QTBAMF_LDFLAGS}
    )

# Install
set(IMPORT_INSTALL_DIR lib/qt4/imports/UnityPlaces)

install(TARGETS UnityPlaces
    LIBRARY DESTINATION ${IMPORT_INSTALL_DIR}
    )

install(FILES qmldir
    DESTINATION ${IMPORT_INSTALL_DIR}
    )


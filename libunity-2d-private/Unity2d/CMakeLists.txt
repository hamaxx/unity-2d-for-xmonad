# Dependencies
pkg_check_modules(WNCK REQUIRED libwnck-1.0)
pkg_check_modules(QTBAMF REQUIRED libqtbamf)
pkg_check_modules(QTGCONF REQUIRED libqtgconf)

# Sources
set(unity-2d-private-qml_SRCS
    plugin.cpp
    qsortfilterproxymodelqml.cpp
    blendedimageprovider.cpp
    windowimageprovider.cpp
    windowinfo.cpp
    windowslist.cpp
    screeninfo.cpp
    cacheeffect.cpp
    workspacesinfo.cpp
    signalwaiter.cpp
    dragitem.cpp
    dragitemwithurl.cpp
    dropitem.cpp
    )

set(unity-2d-private-qml_MOC_HDRS
    plugin.h
    qsortfilterproxymodelqml.h
    windowinfo.h
    windowslist.h
    screeninfo.h
    cacheeffect.h
    workspacesinfo.h
    signalwaiter.h
    dragitem.h
    dragitemwithurl.h
    dropitem.h
    )

qt4_wrap_cpp(unity-2d-private-qml_MOC_SRCS ${unity-2d-private-qml_MOC_HDRS})

# Build
add_library(unity-2d-private-qml SHARED ${unity-2d-private-qml_SRCS} ${unity-2d-private-qml_MOC_SRCS})
add_definitions(-DWNCK_I_KNOW_THIS_IS_UNSTABLE)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${WNCK_INCLUDE_DIRS}
    ${QTBAMF_INCLUDE_DIRS}
    ${QTGCONF_INCLUDE_DIRS}
    ${libunity-2d-private_SOURCE_DIR}/src
    )

target_link_libraries(unity-2d-private-qml
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTDBUS_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${WNCK_LDFLAGS}
    ${QTBAMF_LDFLAGS}
    ${QTGCONF_LDFLAGS}
    unity-2d-private
    )

# Install
set(IMPORT_INSTALL_DIR lib/qt4/imports/Unity2d)

install(TARGETS unity-2d-private-qml
    LIBRARY DESTINATION ${IMPORT_INSTALL_DIR}
    )

install(FILES qmldir
    DESTINATION ${IMPORT_INSTALL_DIR}
    )


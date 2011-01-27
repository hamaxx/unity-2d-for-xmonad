# Dependencies
pkg_check_modules(WNCK REQUIRED libwnck-1.0)
pkg_check_modules(QTBAMF REQUIRED libqtbamf)

# Sources
set(Unity2dQml_SRCS
    plugin.cpp
    qsortfilterproxymodelqml.cpp
    blendedimageprovider.cpp
    windowimageprovider.cpp
    windowinfo.cpp
    windowslist.cpp
    cacheeffect.cpp
    )

set(Unity2dQml_MOC_HDRS
    plugin.h
    qsortfilterproxymodelqml.h
    windowinfo.h
    windowslist.h
    )

qt4_wrap_cpp(Unity2dQml_MOC_SRCS ${Unity2dQml_MOC_HDRS})

# Build
add_library(Unity2dQml SHARED ${Unity2dQml_SRCS} ${Unity2dQml_MOC_SRCS})
add_definitions(-DWNCK_I_KNOW_THIS_IS_UNSTABLE)

include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${WNCK_INCLUDE_DIRS}
    ${QTBAMF_INCLUDE_DIRS}
    )

target_link_libraries(Unity2dQml
    ${QT_QTCORE_LIBRARIES}
    ${QT_QTDBUS_LIBRARIES}
    ${QT_QTDECLARATIVE_LIBRARIES}
    ${WNCK_LDFLAGS}
    ${QTBAMF_LDFLAGS}
    )

# Install
set(IMPORT_INSTALL_DIR lib/qt4/imports/Unity2d)

install(TARGETS Unity2dQml
    LIBRARY DESTINATION ${IMPORT_INSTALL_DIR}
    )

install(FILES qmldir
    DESTINATION ${IMPORT_INSTALL_DIR}
    )


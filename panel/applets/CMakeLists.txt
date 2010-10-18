project(uqapplets)

# Dependencies
include(FindPkgConfig)

pkg_check_modules(QTBAMF REQUIRED libqtbamf)
pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)

# Sources
set(uqapplets_SRCS
    appindicator/appindicatorapplet.cpp
    appindicator/sniitem.cpp
    appmenu/appmenuapplet.cpp
    appmenu/registrar.cpp
    appname/appnameapplet.cpp
    homebutton/homebuttonapplet.cpp
    indicator/abstractindicator.cpp
    indicator/datetimeindicator.cpp
    indicator/indicatorapplet.cpp
    indicator/indicatorservicemanager.cpp
    legacytray/legacytrayapplet.cpp
    legacytray/fdoselectionmanager.cpp
    legacytray/fdotask.cpp
    )

qt4_add_dbus_adaptor(uqapplets_SRCS appmenu/org.ayatana.AppMenu.Registrar.xml
    registrar.h Registrar
    )

qt4_automoc(${uqapplets_SRCS})

# Build
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/appindicator
    ${CMAKE_CURRENT_SOURCE_DIR}/appmenu
    ${CMAKE_CURRENT_SOURCE_DIR}/appname
    ${CMAKE_CURRENT_SOURCE_DIR}/homebutton
    ${CMAKE_CURRENT_SOURCE_DIR}/indicator
    ${QTBAMF_INCLUDE_DIRS}
    ${DBUSMENUQT_INCLUDE_DIRS}
    ${CMAKE_CURRENT_BINARY_DIR}
    )

add_library(uqapplets SHARED ${uqapplets_SRCS})

target_link_libraries(uqapplets
    ${QT_QTGUI_LIBRARIES}
    ${QT_QTCORE_LIBRARIES}
    ${DBUSMENUQT_LDFLAGS}
    ${QTBAMF_LDFLAGS}
    uqpanel
    )

# Install
install(TARGETS uqapplets
    LIBRARY DESTINATION lib${LIB_SUFFIX}
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    )

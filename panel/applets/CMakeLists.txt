include(FindPkgConfig)

pkg_check_modules(QTBAMF REQUIRED libqtbamf)
pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)

add_subdirectory(appindicator)
add_subdirectory(appmenu)
add_subdirectory(appname)
add_subdirectory(indicator)
add_subdirectory(homebutton)
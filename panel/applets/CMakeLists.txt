include(FindPkgConfig)

pkg_check_modules(QTBAMF REQUIRED libqtbamf)
pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)

add_subdirectory(appmenu)
add_subdirectory(appname)

project(libunity-2d-private)

# Dependencies
pkg_check_modules(QTBAMF REQUIRED libqtbamf)
pkg_check_modules(QTGCONF REQUIRED libqtgconf)
pkg_check_modules(QTDEE REQUIRED libqtdee)
pkg_check_modules(DBUSMENUQT REQUIRED dbusmenu-qt)
pkg_check_modules(STARTUPNOTIFICATION REQUIRED libstartup-notification-1.0)
pkg_check_modules(INDICATOR REQUIRED indicator3-0.4)
pkg_check_modules(DCONFQT REQUIRED dconf-qt)
pkg_check_modules(UNITYCORE REQUIRED unity-core-4.0)
pkg_check_modules(DEE REQUIRED dee-1.0)
pkg_check_modules(XINPUT REQUIRED xi)

set(libunity-2d-private_SOVERSION 0)
set(libunity-2d-private_VERSION ${libunity-2d-private_SOVERSION}.0.0)

add_subdirectory(src)
add_subdirectory(Unity2d)
add_subdirectory(tests)

TEMPLATE = lib
CONFIG += qt link_pkgconfig
QT += declarative
PKGCONFIG = gdk-2.0 gio-2.0 libwnck-1.0 libqtbamf libqtgconf
DEFINES += WNCK_I_KNOW_THIS_IS_UNSTABLE

TARGET = UnityApplications

OBJECTS_DIR = tmp
MOC_DIR = tmp

HEADERS += \
    plugin.h \
    launcherapplication.h \
    iconimageprovider.h \
    launcherapplicationslist.h \
    launchertooltip.h

SOURCES += \
    plugin.cpp \
    launcherapplication.cpp \
    iconimageprovider.cpp \
    launcherapplicationslist.cpp \
    launchertooltip.cpp

INSTALL_PATH = /usr/lib/qt4/imports/$$TARGET

target.path = $$INSTALL_PATH

qmldir.files = qmldir
qmldir.path = $$INSTALL_PATH

INSTALLS = target qmldir


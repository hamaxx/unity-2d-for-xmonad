TEMPLATE = lib
CONFIG += qt link_pkgconfig
QT += declarative
PKGCONFIG = gdk-2.0 gio-2.0 libwnck-1.0 libqtbamf libqtgconf

TARGET = launcher_plugin

DESTDIR = lib
OBJECTS_DIR = tmp
MOC_DIR = tmp

HEADERS += \
    launcherplugin.h \
    launcherapplication.h \
    iconimageprovider.h \
    launcherapplicationslist.h

SOURCES += \
    launcherplugin.cpp \
    launcherapplication.cpp \
    iconimageprovider.cpp \
    launcherapplicationslist.cpp

INSTALL_PATH = /usr/lib/qt4/imports/launcher

target.path = $$INSTALL_PATH

qmldir.files = qmldir
qmldir.path = $$INSTALL_PATH

INSTALLS = target qmldir


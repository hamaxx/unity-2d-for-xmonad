TEMPLATE = lib
CONFIG += qt link_pkgconfig
QT += declarative
PKGCONFIG = gtk+-2.0 gio-2.0
INCLUDEPATH = ../../bamf-qt/src/libQtBamf

TARGET = launcher_plugin

DESTDIR = lib
OBJECTS_DIR = tmp
MOC_DIR = tmp


HEADERS += \
    launcherplugin.h \
    launcherapplication.h \
    iconimageprovider.h

SOURCES += \
    launcherplugin.cpp \
    launcherapplication.cpp \
    iconimageprovider.cpp

TEMPLATE = lib
CONFIG += qt link_pkgconfig
QT += declarative
PKGCONFIG = launcher-0.3 gconf-2.0

DESTDIR = lib
OBJECTS_DIR = tmp
MOC_DIR = tmp


HEADERS += \
    liblauncherplugin.h \
    launcherapplication.h

SOURCES += \
    liblauncherplugin.cpp \
    launcherapplication.cpp

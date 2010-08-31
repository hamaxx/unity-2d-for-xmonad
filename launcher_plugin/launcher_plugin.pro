TEMPLATE = lib
CONFIG += qt link_pkgconfig
QT += declarative
PKGCONFIG = gdk-2.0 gio-2.0 libwnck-1.0
INCLUDEPATH += ../../bamf-qt/src/libQtBamf ../../libQtGConf
LIBS += -L../../bamf-qt/src/libQtBamf -lQtBamf -L../../libQtGConf -lQtGConf

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

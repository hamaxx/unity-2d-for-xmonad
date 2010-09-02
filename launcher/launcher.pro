# Copyright (C) 2010 Canonical, Ltd.
#
# Authors:
#  Olivier Tilloy <olivier.tilloy@canonical.com>
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; version 3.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

TEMPLATE = app
CONFIG += qt link_pkgconfig
QT += declarative
PKGCONFIG = x11

TARGET = unity-qt-launcher

DESTDIR = bin
OBJECTS_DIR = tmp
MOC_DIR = tmp

SOURCES += launcher.cpp

target.path = /usr/bin
INSTALLS = target


/*
 * Copyright (C) 2012 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// Handy macros to declare GObject callbacks. The 'n' in CALLBACKn refers to
// the number of dummy arguments the callback returns
#define GOBJECT_CALLBACK0(callbackName, slot) \
static void \
callbackName(GObject* src, QObject* dst) \
{ \
    QMetaObject::invokeMethod(dst, slot); \
}

#define GOBJECT_CALLBACK1(callbackName, slot) \
static void \
callbackName(GObject* src, void* dummy1, QObject* dst) \
{ \
    QMetaObject::invokeMethod(dst, slot); \
}

#define GOBJECT_CALLBACK2(callbackName, slot) \
static void \
callbackName(GObject* src, void* dummy1, void* dummy2, QObject* dst) \
{ \
    QMetaObject::invokeMethod(dst, slot); \
}

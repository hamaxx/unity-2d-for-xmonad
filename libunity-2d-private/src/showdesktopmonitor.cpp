/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * License: GPL v3
 */

// own header
#include "showdesktopmonitor.h"

// libwnck
extern "C" {
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
}

#define GOBJECT_CALLBACK0(callbackName, slot) \
static void \
callbackName(GObject* src, QObject* dst) \
{ \
    QMetaObject::invokeMethod(dst, slot); \
}

GOBJECT_CALLBACK0(showingDesktopChangedCB, "onShowingDesktopChanged");

ShowDesktopMonitor::ShowDesktopMonitor()
{
    WnckScreen* screen = wnck_screen_get_default();
    g_signal_connect(G_OBJECT(screen), "showing_desktop_changed", G_CALLBACK(showingDesktopChangedCB), this);
}

ShowDesktopMonitor::~ShowDesktopMonitor()
{
    WnckScreen* screen = wnck_screen_get_default();
    g_signal_handlers_disconnect_by_func(G_OBJECT(screen), gpointer(showingDesktopChangedCB), this);
}

bool ShowDesktopMonitor::shown() const
{
    WnckScreen* screen = wnck_screen_get_default();
    return wnck_screen_get_showing_desktop(screen);
}

void ShowDesktopMonitor::onShowingDesktopChanged()
{
    Q_EMIT shownChanged();
}

#include "showdesktopmonitor.moc"

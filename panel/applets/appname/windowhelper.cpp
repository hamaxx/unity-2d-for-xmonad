/*
 * Plasma applet to display DBus global menu
 *
 * Copyright 2009 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
// Self
#include "windowhelper.h"

// Local

// unity-qt
#include <debug_p.h>

// libwnck
#undef signals
extern "C" {
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
}

// Qt
#include <QDateTime>

struct WindowHelperPrivate
{
    uint m_xid;
    WnckWindow* m_window;
};

WindowHelper::WindowHelper(QObject* parent)
: QObject(parent)
, d(new WindowHelperPrivate)
{
    d->m_window = 0;

    WnckScreen* screen = wnck_screen_get_default();
    wnck_screen_force_update(screen);
}

WindowHelper::~WindowHelper()
{
    delete d;
}

static void stateChangedCB(GObject* window,
    WnckWindowState changed_mask,
    WnckWindowState new_state,
    WindowHelper*  watcher)
{
    QMetaObject::invokeMethod(watcher, "stateChanged");
}

void WindowHelper::setXid(uint xid)
{
    if (d->m_window) {
        g_signal_handlers_disconnect_by_func(d->m_window, gpointer(stateChangedCB), this);
        d->m_window = 0;
    }
    if (xid != 0) {
        d->m_window = wnck_window_get(xid);
        g_signal_connect(G_OBJECT(d->m_window), "state-changed", G_CALLBACK(stateChangedCB), this);
    }
}

bool WindowHelper::isMaximized() const
{
    if (!d->m_window) {
        return false;
    }
    return wnck_window_is_maximized(d->m_window);
}

void WindowHelper::close()
{
    guint32 timestamp = QDateTime::currentDateTime().toTime_t();
    wnck_window_close(d->m_window, timestamp);
}

void WindowHelper::minimize()
{
    wnck_window_minimize(d->m_window);
}

void WindowHelper::unmaximize()
{
    wnck_window_unmaximize(d->m_window);
}

#include "windowhelper.moc"

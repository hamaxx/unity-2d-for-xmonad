/*
 * Plasma applet to display DBus global menu
 *
 * Copyright 2009 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
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

// Self
#include "windowhelper.h"

// Local

// unity-2d
#include <debug_p.h>

// Bamf
#include <bamf-matcher.h>
#include <bamf-window.h>

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
    WnckWindow* m_window;
};

WindowHelper::WindowHelper(QObject* parent)
: QObject(parent)
, d(new WindowHelperPrivate)
{
    d->m_window = 0;

    WnckScreen* screen = wnck_screen_get_default();
    wnck_screen_force_update(screen);

    update();

    connect(&BamfMatcher::get_default(), SIGNAL(ActiveWindowChanged(BamfWindow*,BamfWindow*)),
        SLOT(update()));
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

static void nameChangedCB(GObject* window,
    WindowHelper*  watcher)
{
    QMetaObject::invokeMethod(watcher, "nameChanged");
}

void WindowHelper::update()
{
    BamfWindow* bamfWindow = BamfMatcher::get_default().active_window();
    uint xid = bamfWindow ? bamfWindow->xid() : 0;

    if (d->m_window) {
        g_signal_handlers_disconnect_by_func(d->m_window, gpointer(stateChangedCB), this);
        g_signal_handlers_disconnect_by_func(d->m_window, gpointer(nameChangedCB), this);
        d->m_window = 0;
    }
    if (xid != 0) {
        d->m_window = wnck_window_get(xid);
        g_signal_connect(G_OBJECT(d->m_window), "name-changed", G_CALLBACK(nameChangedCB), this);
        g_signal_connect(G_OBJECT(d->m_window), "state-changed", G_CALLBACK(stateChangedCB), this);
    }
    stateChanged();
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

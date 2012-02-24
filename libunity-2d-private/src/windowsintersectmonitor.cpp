/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 * - Florian Boucault <florian.boucault@canonical.com>
 * - Ugo Riboni <ugo.riboni@canonical.com>
 *
 * License: GPL v3
 */

// Self
#include "windowsintersectmonitor.h"

// libunity-2d
#include <debug_p.h>
#include "gobjectcallback.h"

// Qt
#include <QCursor>
#include <QEvent>
#include <QTimer>
#include <QWidget>
#include <QDesktopWidget>

// libwnck
extern "C" {
#include <libwnck/libwnck.h>
}

// Screen callbacks
GOBJECT_CALLBACK1(activeWindowChangedCB, "updateActiveWindowConnections");
GOBJECT_CALLBACK1(activeWorkspaceChangedCB, "updateIntersect");
GOBJECT_CALLBACK0(showingDesktopChangedCB, "updateIntersect");

// Window callbacks
GOBJECT_CALLBACK2(stateChangedCB, "updateIntersect");
GOBJECT_CALLBACK0(geometryChangedCB, "updateIntersect");
GOBJECT_CALLBACK0(workspaceChangedCB, "updateIntersect");

WindowsIntersectMonitor::WindowsIntersectMonitor()
    : QObject()
    , m_activeWindow(0)
{
    WnckScreen* screen = wnck_screen_get_default();
    g_signal_connect(G_OBJECT(screen), "active-window-changed", G_CALLBACK(activeWindowChangedCB), this);
    g_signal_connect(G_OBJECT(screen), "active-workspace-changed", G_CALLBACK(activeWorkspaceChangedCB), this);
    g_signal_connect(G_OBJECT(screen), "showing-desktop-changed", G_CALLBACK(showingDesktopChangedCB), this);

    updateActiveWindowConnections();
}

WindowsIntersectMonitor::~WindowsIntersectMonitor()
{
    disconnectFromGSignals();
    WnckScreen* screen = wnck_screen_get_default();
    g_signal_handlers_disconnect_by_func(G_OBJECT(screen), gpointer(activeWindowChangedCB), this);
    g_signal_handlers_disconnect_by_func(G_OBJECT(screen), gpointer(activeWorkspaceChangedCB), this);
    g_signal_handlers_disconnect_by_func(G_OBJECT(screen), gpointer(showingDesktopChangedCB), this);
}

void WindowsIntersectMonitor::disconnectFromGSignals()
{
    if (m_activeWindow) {
        g_signal_handlers_disconnect_by_func(m_activeWindow, gpointer(stateChangedCB), this);
        g_signal_handlers_disconnect_by_func(m_activeWindow, gpointer(geometryChangedCB), this);
        g_signal_handlers_disconnect_by_func(m_activeWindow, gpointer(workspaceChangedCB), this);
    }
}

void WindowsIntersectMonitor::updateActiveWindowConnections()
{
    WnckScreen* screen = wnck_screen_get_default();

    disconnectFromGSignals();
    m_activeWindow = 0;

    WnckWindow* window = wnck_screen_get_active_window(screen);
    if (window) {
        m_activeWindow = window;
        g_signal_connect(G_OBJECT(window), "state-changed", G_CALLBACK(stateChangedCB), this);
        g_signal_connect(G_OBJECT(window), "geometry-changed", G_CALLBACK(geometryChangedCB), this);
        g_signal_connect(G_OBJECT(window), "workspace-changed", G_CALLBACK(workspaceChangedCB), this);
    }

    updateIntersect();
}

void WindowsIntersectMonitor::updateIntersect()
{
    int thisProcessPid = getpid();

    WnckScreen* screen = wnck_screen_get_default();
    WnckWorkspace* workspace = wnck_screen_get_active_workspace(screen);

    // Check whether a window is crossing our rect
    bool crossWindow = false;
    if (!wnck_screen_get_showing_desktop(screen)) {
        GList* list = wnck_screen_get_windows(screen);
        for (; list; list = g_list_next(list)) {
            WnckWindow* window = WNCK_WINDOW(list->data);
            if (wnck_window_is_on_workspace(window, workspace) &&
                wnck_window_get_pid(window) != thisProcessPid) {
                WnckWindowType type = wnck_window_get_window_type(window);

                // Only take into account typical application windows
                if (type != WNCK_WINDOW_NORMAL  &&
                    type != WNCK_WINDOW_DIALOG  &&
                    type != WNCK_WINDOW_TOOLBAR &&
                    type != WNCK_WINDOW_MENU    &&
                    type != WNCK_WINDOW_UTILITY) {
                    continue;
                }

                WnckWindowState state = wnck_window_get_state(window);

                // Skip hidden (==minimized and other states) windows
                if (state & WNCK_WINDOW_STATE_HIDDEN) {
                    continue;
                }

                // Check the window rect
                int x, y, width, height;
                wnck_window_get_geometry(window, &x, &y, &width, &height);
                QRectF rect(x, y, width, height);
                if (rect.intersects(m_monitoredArea)) {
                    crossWindow = true;
                    break;
                }
            }
        }
    }

    if (crossWindow != m_intersects) {
        m_intersects = crossWindow;
        Q_EMIT intersectsChanged();
    }
}

QRectF WindowsIntersectMonitor::monitoredArea() const
{
    return m_monitoredArea;
}

void WindowsIntersectMonitor::setMonitoredArea(const QRectF& monitoredArea)
{
    if (m_monitoredArea != monitoredArea) {
        m_monitoredArea = monitoredArea;
        Q_EMIT monitoredAreaChanged();
        updateIntersect();
    }
}

bool WindowsIntersectMonitor::intersects() const
{
    return m_intersects;
}

#include "windowsintersectmonitor.moc"

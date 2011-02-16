/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
// Self
#include "intellihidecontroller.h"

// Local

// libunity-2d
#include <debug_p.h>
#include <mousearea.h>
#include <unity2dpanel.h>

// Qt
#include <QEvent>
#include <QPropertyAnimation>

// libwnck
#undef signals
extern "C" {
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
}

static const int SLIDE_DURATION = 500;

static void
updateActiveWindowConnectionsCB(GObject* screen, void* dummy, IntellihideController* controller)
{
    QMetaObject::invokeMethod(controller, "updateActiveWindowConnections");
}

static void
updateVisibilityCB(GObject* screen, IntellihideController* controller)
{
    QMetaObject::invokeMethod(controller, "updateVisibility");
}

static void
stateChangedCB(GObject* screen, WnckWindowState*, WnckWindowState*, IntellihideController* controller)
{
    QMetaObject::invokeMethod(controller, "updateVisibility");
}

IntellihideController::IntellihideController(Unity2dPanel* panel)
: m_panel(panel)
, m_mouseArea(new MouseArea(this))
, m_slideAnimation(new QPropertyAnimation(this))
, m_activeWindow(0)
, m_visibility(VisiblePanel)
{
    connect(m_mouseArea, SIGNAL(entered()), SLOT(forceVisiblePanel()));

    m_panel->setUseStrut(false);
    m_panel->installEventFilter(this);

    m_slideAnimation->setTargetObject(m_panel);
    m_slideAnimation->setPropertyName("delta");
    m_slideAnimation->setDuration(SLIDE_DURATION);
    m_slideAnimation->setEasingCurve(QEasingCurve::InOutCubic);

    WnckScreen* screen = wnck_screen_get_default();
    g_signal_connect(G_OBJECT(screen), "active-window-changed", G_CALLBACK(updateActiveWindowConnectionsCB), this);
    g_signal_connect(G_OBJECT(screen), "active-workspace-changed", G_CALLBACK(updateVisibilityCB), this);

    updateActiveWindowConnections();
}

IntellihideController::~IntellihideController()
{
}

void IntellihideController::updateActiveWindowConnections()
{
    WnckScreen* screen = wnck_screen_get_default();

    if (m_activeWindow) {
        g_signal_handlers_disconnect_by_func(m_activeWindow, gpointer(updateVisibilityCB), this);
        g_signal_handlers_disconnect_by_func(m_activeWindow, gpointer(stateChangedCB), this);
        m_activeWindow = 0;
    }

    WnckWindow* window = wnck_screen_get_active_window(screen);
    if (window) {
        m_activeWindow = window;
        g_signal_connect(G_OBJECT(window), "state-changed", G_CALLBACK(stateChangedCB), this);
        g_signal_connect(G_OBJECT(window), "geometry-changed", G_CALLBACK(updateVisibilityCB), this);
        g_signal_connect(G_OBJECT(window), "workspace-changed", G_CALLBACK(updateVisibilityCB), this);
    }

    updateVisibility();
}

void IntellihideController::updateVisibility()
{
    if (m_visibility == ForceVisiblePanel) {
        return;
    }
    int launcherPid = getpid();

    // Compute launcherRect, adjust "left" to the position where the launcher
    // is fully visible.
    QRect launcherRect = m_panel->geometry();
    launcherRect.moveLeft(0);

    WnckScreen* screen = wnck_screen_get_default();
    WnckWorkspace* workspace = wnck_screen_get_active_workspace(screen);

    // Check whether a window is crossing our launcher rect
    bool crossWindow = false;
    GList* list = wnck_screen_get_windows(screen);
    for (; list; list = g_list_next(list)) {
        WnckWindow* window = WNCK_WINDOW(list->data);
        if (wnck_window_is_on_workspace(window, workspace) && wnck_window_get_pid(window) != launcherPid) {
            // Maximized window should always be considered as crossing the
            // window
            WnckWindowState state = wnck_window_get_state(window);
            if (state & WNCK_WINDOW_STATE_MAXIMIZED_HORIZONTALLY) {
                crossWindow = true;
                break;
            }

            // Not maximized => really check the window rect
            int x, y, width, height;
            wnck_window_get_geometry(window, &x, &y, &width, &height);
            QRect rect(x, y, width, height);
            if (rect.intersects(launcherRect)) {
                crossWindow = true;
                break;
            }
        }
    }

    m_visibility = crossWindow ? HiddenPanel : VisiblePanel;
    slidePanel();
}

bool IntellihideController::eventFilter(QObject* object, QEvent* event)
{
    if (event->type() == QEvent::Leave && !m_mouseArea->containsMouse() && m_visibility == ForceVisiblePanel) {
        m_visibility = VisiblePanel;
        updateVisibility();
    } else if (event->type() == QEvent::Resize) {
        QRect rect = m_panel->geometry();
        rect.setWidth(1);
        m_mouseArea->setGeometry(rect);

        m_slideAnimation->setStartValue(-m_panel->width());
        m_slideAnimation->setEndValue(0);
    }
    return false;
}

void IntellihideController::slidePanel()
{
    bool visible = m_visibility != HiddenPanel;
    if (visible && m_panel->delta() == 0) {
        return;
    }
    if (!visible && m_panel->delta() == -m_panel->width()) {
        return;
    }
    m_slideAnimation->setDirection(visible ? QAbstractAnimation::Forward : QAbstractAnimation::Backward);
    if (m_slideAnimation->state() == QAbstractAnimation::Stopped) {
        m_slideAnimation->start();
    }
}

void IntellihideController::forceVisiblePanel()
{
    if (m_visibility != ForceVisiblePanel) {
        m_visibility = ForceVisiblePanel;
        slidePanel();
    }
}

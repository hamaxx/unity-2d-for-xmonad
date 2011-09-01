/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 * - Florian Boucault <florian.boucault@canonical.com>
 *
 * License: GPL v3
 */
// Self
#include "intellihidebehavior.h"

// Local
#include "unity2dpanel.h"

// libunity-2d
#include <debug_p.h>
#include <edgehitdetector.h>

// Qt
#include <QCursor>
#include <QEvent>
#include <QTimer>
#include <QWidget>
#include <QDesktopWidget>

// libwnck
extern "C" {
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
}

static const int AUTOHIDE_TIMEOUT = 1000;

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

// Screen callbacks
GOBJECT_CALLBACK1(activeWindowChangedCB, "updateActiveWindowConnections");
GOBJECT_CALLBACK1(activeWorkspaceChangedCB, "updateVisibility");

// Window callbacks
GOBJECT_CALLBACK2(stateChangedCB, "updateVisibility");
GOBJECT_CALLBACK0(geometryChangedCB, "updateVisibility");
GOBJECT_CALLBACK0(workspaceChangedCB, "updateVisibility");

IntelliHideBehavior::IntelliHideBehavior(QWidget* panel)
: AbstractVisibilityBehavior(panel)
, m_updateVisibilityTimer(new QTimer(this))
, m_edgeHitDetector(0)
, m_activeWindow(0)
{
    m_updateVisibilityTimer->setSingleShot(true);
    m_updateVisibilityTimer->setInterval(AUTOHIDE_TIMEOUT);
    connect(m_updateVisibilityTimer, SIGNAL(timeout()), SLOT(updateVisibility()));

    setPanel(panel);

    WnckScreen* screen = wnck_screen_get_default();
    g_signal_connect(G_OBJECT(screen), "active-window-changed", G_CALLBACK(activeWindowChangedCB), this);
    g_signal_connect(G_OBJECT(screen), "active-workspace-changed", G_CALLBACK(activeWorkspaceChangedCB), this);

    updateActiveWindowConnections();
}

IntelliHideBehavior::~IntelliHideBehavior()
{
    disconnectFromGSignals();
    WnckScreen* screen = wnck_screen_get_default();
    g_signal_handlers_disconnect_by_func(G_OBJECT(screen), gpointer(activeWindowChangedCB), this);
    g_signal_handlers_disconnect_by_func(G_OBJECT(screen), gpointer(activeWorkspaceChangedCB), this);
}

void IntelliHideBehavior::disconnectFromGSignals()
{
    if (m_activeWindow) {
        g_signal_handlers_disconnect_by_func(m_activeWindow, gpointer(stateChangedCB), this);
        g_signal_handlers_disconnect_by_func(m_activeWindow, gpointer(geometryChangedCB), this);
        g_signal_handlers_disconnect_by_func(m_activeWindow, gpointer(workspaceChangedCB), this);
    }
}

void IntelliHideBehavior::updateActiveWindowConnections()
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

    updateVisibility();
}

void IntelliHideBehavior::updateVisibility()
{
    if (m_panel == NULL) {
        return;
    }
    if (isMouseForcingVisibility()) {
        return;
    }
    int launcherPid = getpid();

    // Compute launcherRect, adjust "left" to the position where the launcher
    // is fully visible.
    QRect launcherRect = m_panel->geometry();
    // FIXME: the following code assumes that the launcher is on the left edge
    // of the screen
    if (QApplication::isLeftToRight()) {
        launcherRect.moveLeft(0);
    } else {
        QDesktopWidget* desktop = QApplication::desktop();
        const QRect screen = desktop->screenGeometry(m_panel);
        launcherRect.moveRight(screen.right());
    }

    WnckScreen* screen = wnck_screen_get_default();
    WnckWorkspace* workspace = wnck_screen_get_active_workspace(screen);

    // Check whether a window is crossing our launcher rect
    bool crossWindow = false;
    GList* list = wnck_screen_get_windows(screen);
    for (; list; list = g_list_next(list)) {
        WnckWindow* window = WNCK_WINDOW(list->data);
        if (wnck_window_is_on_workspace(window, workspace) && wnck_window_get_pid(window) != launcherPid) {
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
            QRect rect(x, y, width, height);
            if (rect.intersects(launcherRect)) {
                crossWindow = true;
                break;
            }
        }
    }

    if (crossWindow) {
        hidePanel();
    } else {
        showPanel();
    }
}

bool IntelliHideBehavior::eventFilter(QObject* object, QEvent* event)
{
    switch (event->type()) {
    case QEvent::Enter:
        m_updateVisibilityTimer->stop();
        break;
    case QEvent::Leave:
        m_updateVisibilityTimer->start();
        break;
    default:
        break;
    }
    return false;
}

bool IntelliHideBehavior::isMouseForcingVisibility() const
{
    // We check the cursor position ourself because using QWidget::underMouse()
    // is unreliable. It causes LP bug #740280
    return m_panel != NULL && m_panel->geometry().contains(QCursor::pos());
}

void IntelliHideBehavior::hidePanel()
{
    m_visible = false;
    Q_EMIT visibleChanged(m_visible);
    createEdgeHitDetector();
}

void IntelliHideBehavior::showPanel()
{
    // Delete the edge hit detector so that it does not prevent mouse events
    // from reaching the panel
    delete m_edgeHitDetector;
    m_edgeHitDetector = 0;
    m_visible = true;
    Q_EMIT visibleChanged(m_visible);
}

void IntelliHideBehavior::createEdgeHitDetector()
{
    m_edgeHitDetector = new EdgeHitDetector(this);
    connect(m_edgeHitDetector, SIGNAL(edgeHit()), SLOT(showPanel()));
}

void IntelliHideBehavior::setPanel(QWidget *panel)
{
    if (m_panel != NULL) {
        m_panel->removeEventFilter(this);
    }
    AbstractVisibilityBehavior::setPanel(panel);
    if (m_panel != NULL) {
        m_panel->installEventFilter(this);
    }
}

#include "intellihidebehavior.moc"

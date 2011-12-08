/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 * - Florian Boucault <florian.boucault@canonical.com>
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
#include "autohidebehavior.h"

// Local
#include <edgehitdetector.h>

// libunity-2d
#include <debug_p.h>

// Qt
#include <QCursor>
#include <QTimer>
#include <QWidget>

// libwnck
extern "C" {
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
}

static const int AUTOHIDE_TIMEOUT = 1000;

#define GOBJECT_CALLBACK0(callbackName, slot) \
static void \
callbackName(GObject* src, QObject* dst) \
{ \
    QMetaObject::invokeMethod(dst, slot); \
}

GOBJECT_CALLBACK0(showingDesktopChangedCB, "onShowingDesktopChanged");

AutoHideBehavior::AutoHideBehavior(QWidget* panel)
: AbstractVisibilityBehavior(panel)
, m_autohideTimer(new QTimer(this))
, m_edgeHitDetector(0)
{
    m_autohideTimer->setSingleShot(true);
    m_autohideTimer->setInterval(AUTOHIDE_TIMEOUT);
    connect(m_autohideTimer, SIGNAL(timeout()), SLOT(hidePanel()));

    setPanel(panel);

    WnckScreen* screen = wnck_screen_get_default();
    g_signal_connect(G_OBJECT(screen), "showing_desktop_changed", G_CALLBACK(showingDesktopChangedCB), this);
}

AutoHideBehavior::~AutoHideBehavior()
{
    WnckScreen* screen = wnck_screen_get_default();
    g_signal_handlers_disconnect_by_func(G_OBJECT(screen), gpointer(showingDesktopChangedCB), this);
}

bool AutoHideBehavior::eventFilter(QObject*, QEvent* event)
{
    switch (event->type()) {
    case QEvent::Enter:
        m_autohideTimer->stop();
        m_visible = true;
        Q_EMIT visibleChanged(m_visible);
        break;
    case QEvent::Leave:
        m_autohideTimer->start();
        break;
    default:
        break;
    }
    return false;
}

void AutoHideBehavior::hidePanel()
{
    m_visible = false;
    Q_EMIT visibleChanged(m_visible);
    createEdgeHitDetector();
}

void AutoHideBehavior::showPanel()
{
    // Delete the mouse area so that it does not prevent mouse events from
    // reaching the panel
    delete m_edgeHitDetector;
    m_edgeHitDetector = 0;
    m_autohideTimer->stop();
    m_visible = true;
    Q_EMIT visibleChanged(m_visible);
}

void AutoHideBehavior::onShowingDesktopChanged()
{
    WnckScreen* screen = wnck_screen_get_default();
    gboolean isShowingDesktop = wnck_screen_get_showing_desktop(screen);
    if (isShowingDesktop) {
        showPanel();
        m_autohideTimer->start();
    } else {
        hidePanel();
    }
}


void AutoHideBehavior::createEdgeHitDetector()
{
    m_edgeHitDetector = new EdgeHitDetector(this);
    connect(m_edgeHitDetector, SIGNAL(edgeHit()), SLOT(showPanel()));
}


void AutoHideBehavior::setPanel(QWidget *panel)
{
    if (m_panel != NULL) {
        m_panel->removeEventFilter(this);
    }
    AbstractVisibilityBehavior::setPanel(panel);
    if (m_panel != NULL) {
        m_panel->installEventFilter(this);
        if (!m_panel->geometry().contains(QCursor::pos())) {
            m_autohideTimer->start();
        }
    }
}

#include "autohidebehavior.moc"

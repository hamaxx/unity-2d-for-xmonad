/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
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
#include "autohidebehavior.h"

// Local
#include <visibilitycontroller.h>

// libunity-2d
#include <debug_p.h>
#include <unity2dpanel.h>
#include <mousearea.h>

// Qt
#include <QCursor>
#include <QTimer>

static const int AUTOHIDE_TIMEOUT = 1000;

AutoHideBehavior::AutoHideBehavior(VisibilityController* controller, Unity2dPanel* panel)
: AbstractVisibilityBehavior(controller, panel)
, m_autohideTimer(new QTimer(this))
{
    m_autohideTimer->setSingleShot(true);
    m_autohideTimer->setInterval(AUTOHIDE_TIMEOUT);
    connect(m_autohideTimer, SIGNAL(timeout()), m_panel, SLOT(slideOut()));

    m_panel->installEventFilter(this);
    if (!m_panel->geometry().contains(QCursor::pos())) {
        if (m_panel->delta() == 0) {
            /* Launcher is fully visible */
            m_autohideTimer->start();
        } else {
            /* Launcher is partially hidden */
            m_panel->slideOut();
        }
    }
}

AutoHideBehavior::~AutoHideBehavior()
{
}

bool AutoHideBehavior::eventFilter(QObject*, QEvent* event)
{
    switch (event->type()) {
    case QEvent::Enter:
        m_autohideTimer->stop();
        break;
    case QEvent::Leave:
        if (!m_controller->isMouseOverHomeButton()) {
            m_autohideTimer->start();
        }
        break;
    default:
        break;
    }
    return false;
}

void AutoHideBehavior::onMouseOverHomeButtonChanged()
{
    if (m_controller->isMouseOverHomeButton()) {
        m_panel->slideIn();
    } else {
        if (!m_panel->underMouse()) {
            m_autohideTimer->start();
        }
    }
}

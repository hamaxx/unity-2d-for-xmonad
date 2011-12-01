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
#include "visibilitycontroller.h"

// Local
#include <autohidebehavior.h>
#include <forcevisiblebehavior.h>
#include <intellihidebehavior.h>

// unity-2d
#include <debug_p.h>
#include <unity2dpanel.h>
#include <config.h>

// Qt
#include <QDBusConnection>
#include <QDBusServiceWatcher>

VisibilityController::VisibilityController(Unity2dPanel* panel)
: QObject(panel)
, m_panel(panel)
, m_dbusWatcher(new QDBusServiceWatcher(this))
{
    m_dbusWatcher->setConnection(QDBusConnection::sessionBus());
    m_dbusWatcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);

    connect(&launcher2dConfiguration(), SIGNAL(hideModeChanged(int)), SLOT(update()));
    connect(m_panel, SIGNAL(manualSlidingChanged(bool)), SLOT(update()));
    connect(m_dbusWatcher, SIGNAL(serviceUnregistered(const QString&)), SLOT(slotServiceUnregistered(const QString&)));
    update();
}

VisibilityController::~VisibilityController()
{
}

void VisibilityController::update(UpdateReason reason)
{
    if (!m_forceVisibleCountHash.isEmpty()) {
        return;
    }
    AutoHideMode mode = AutoHideMode(launcher2dConfiguration().property("hideMode").toInt());

    setBehavior(0);

    /* Do not use any hiding controller if the panel is being slid manually */
    if (!m_panel->manualSliding()) {
        switch (mode) {
        case ManualHide:
            m_panel->setUseStrut(true);
            m_panel->slideIn();
            break;
        case AutoHide:
            m_panel->setUseStrut(false);
            setBehavior(new AutoHideBehavior(m_panel));
            break;
        case IntelliHide:
            m_panel->setUseStrut(false);
            setBehavior(new IntelliHideBehavior(m_panel));
            if (reason == UpdateFromForceVisibilityEnded && !m_panel->geometry().contains(QCursor::pos())) {
                // The first thing IntelliHideBehavior does is checking if there is
                // a window behind the panel, and if there is one, hide the panel immediately
                // This is correct for some cases, but in the case we come from an update because the
                // panel is not forced visible anymore and the mouse is not in the panel,
                // i.e. the launcher was visible and the user clicked in an action of the tile menu,
                // we should still give the user the wait 1 second before hiding behaviour.
                // To achieve this we tell the behaviour controller to show the panel
                // and simulate a mouse leave on the panel to start the hiding timer
                QMetaObject::invokeMethod(m_behavior.data(), "showPanel");
                QEvent e(QEvent::Leave);
                QCoreApplication::sendEvent(m_panel, &e);
            }
            break;
        }
    }
}

void VisibilityController::beginForceVisible(const QString& service)
{
    bool wasEmpty = m_forceVisibleCountHash.isEmpty();
    if (m_forceVisibleCountHash.contains(service)) {
        ++m_forceVisibleCountHash[service];
    } else {
        m_forceVisibleCountHash[service] = 1;
        if (!service.isEmpty()) {
            m_dbusWatcher->addWatchedService(service);
        }
    }
    if (wasEmpty) {
        setBehavior(new ForceVisibleBehavior(m_panel));
    }
}

void VisibilityController::endForceVisible(const QString& service)
{
    if (m_forceVisibleCountHash.contains(service)) {
        if (m_forceVisibleCountHash[service] == 1) {
            m_forceVisibleCountHash.remove(service);
            m_dbusWatcher->removeWatchedService(service);
        } else {
            --m_forceVisibleCountHash[service];
        }
    } else {
        UQ_WARNING << "Application" << service << "called endForceVisible() more than beginForceVisible().";
    }
    if (m_forceVisibleCountHash.isEmpty()) {
        update(UpdateFromForceVisibilityEnded);
    }
}

void VisibilityController::setBehavior(AbstractVisibilityBehavior* behavior)
{
    // This method could be replaced by code calling reset() directly but
    // having only one point where the behavior is changed makes it easy to log
    // behavior changes using something like: UQ_VAR(behavior);
    m_behavior.reset(behavior);
    if (behavior != NULL) {
        connect(behavior, SIGNAL(visibleChanged(bool)), SLOT(updatePanelVisibility(bool)));
        updatePanelVisibility(behavior->visible());
    }
}

void VisibilityController::updatePanelVisibility(bool visible)
{
    if (visible) {
        m_panel->slideIn();
    } else {
        m_panel->slideOut();
    }
}

void VisibilityController::slotServiceUnregistered(const QString& service)
{
    if (!m_forceVisibleCountHash.contains(service)) {
        return;
    }

    UQ_WARNING << "Application" << service << "quit without calling endForceVisible().";
    m_forceVisibleCountHash.remove(service);
    m_dbusWatcher->removeWatchedService(service);
    if (m_forceVisibleCountHash.isEmpty()) {
        update(UpdateFromForceVisibilityEnded);
    }
}

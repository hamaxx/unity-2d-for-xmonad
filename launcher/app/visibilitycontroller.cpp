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

// libqtgconf
#include <gconfitem-qml-wrapper.h>

// Qt
#include <QDBusConnection>
#include <QDBusServiceWatcher>

static const char* GCONF_LAUNCHER_HIDEMODE_KEY = "/desktop/unity-2d/launcher/hide_mode";

VisibilityController::VisibilityController(Unity2dPanel* panel)
: QObject(panel)
, m_panel(panel)
, m_hideModeKey(new GConfItemQmlWrapper(this))
, m_dbusWatcher(new QDBusServiceWatcher(this))
{
    m_hideModeKey->setKey(GCONF_LAUNCHER_HIDEMODE_KEY);

    m_dbusWatcher->setConnection(QDBusConnection::sessionBus());
    m_dbusWatcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);

    connect(m_hideModeKey, SIGNAL(valueChanged()), SLOT(update()));
    connect(m_panel, SIGNAL(useStrutChanged(bool)), SLOT(update()));
    connect(m_panel, SIGNAL(manualSlidingChanged(bool)), SLOT(update()));
    connect(m_dbusWatcher, SIGNAL(serviceUnregistered(const QString&)), SLOT(slotServiceUnregistered(const QString&)));
    update();
}

VisibilityController::~VisibilityController()
{
}

void VisibilityController::update()
{
    if (!m_forceVisibleCountHash.isEmpty()) {
        return;
    }
    AutoHideMode mode = AutoHideMode(m_hideModeKey->getValue().toInt());

    setBehavior(0);

    /* Do not use any hiding controller if the panel is either:
        - being slid manually
        - locked in place (using struts)
    */
    if (!m_panel->manualSliding() && !m_panel->useStrut()) {
        switch (mode) {
        case ManualHide:
            break;
        case AutoHide:
            setBehavior(new AutoHideBehavior(m_panel));
            break;
        case IntelliHide:
            setBehavior(new IntelliHideBehavior(m_panel));
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
        update();
    }
}

void VisibilityController::setBehavior(AbstractVisibilityBehavior* behavior)
{
    // Keep this around: uncommenting it makes it easy to track behavior
    // changes
    //UQ_VAR(behavior);
    m_behavior.reset(behavior);
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
        update();
    }
}

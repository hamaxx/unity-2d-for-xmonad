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

static const char* GCONF_LAUNCHER_HIDEMODE_KEY = "/desktop/unity-2d/launcher/hide_mode";

VisibilityController::VisibilityController(Unity2dPanel* panel)
: QObject(panel)
, m_panel(panel)
, m_hideModeKey(new GConfItemQmlWrapper(this))
, m_forceVisibleCount(0)
{
    m_hideModeKey->setKey(GCONF_LAUNCHER_HIDEMODE_KEY);
    connect(m_hideModeKey, SIGNAL(valueChanged()), SLOT(update()));
    connect(m_panel, SIGNAL(useStrutChanged(bool)), SLOT(update()));
    connect(m_panel, SIGNAL(manualSlidingChanged(bool)), SLOT(update()));
    update();
}

VisibilityController::~VisibilityController()
{
}

void VisibilityController::update()
{
    if (m_forceVisibleCount > 0) {
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

void VisibilityController::beginForceVisible()
{
    m_forceVisibleCount++;
    if (m_forceVisibleCount == 1) {
        setBehavior(new ForceVisibleBehavior(m_panel));
    }
}

void VisibilityController::endForceVisible()
{
    if (m_forceVisibleCount > 0) {
        m_forceVisibleCount--;
    } else {
        UQ_WARNING << "m_forceVisibleCount == 0, this should not happen";
    }
    if (m_forceVisibleCount == 0) {
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

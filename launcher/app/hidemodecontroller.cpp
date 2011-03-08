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
#include "hidemodecontroller.h"

// Local
#include <autohidecontroller.h>
#include <intellihidecontroller.h>

// unity-2d
#include <debug_p.h>
#include <unity2dpanel.h>

// libqtgconf
#include <gconfitem-qml-wrapper.h>

// Qt
#include <QDeclarativeProperty>

static const char* GCONF_LAUNCHER_HIDEMODE_KEY = "/desktop/unity-2d/launcher/hide_mode";

HideModeController::HideModeController(Unity2dPanel* panel, QDeclarativeProperty* property)
: QObject(panel)
, m_panel(panel)
, m_hideModeKey(new GConfItemQmlWrapper(this))
, m_controller(0)
, m_requestAttentionProperty(property)
{
    m_hideModeKey->setKey(GCONF_LAUNCHER_HIDEMODE_KEY);
    connect(m_hideModeKey, SIGNAL(valueChanged()), SLOT(update()));
    connect(m_panel, SIGNAL(useStrutChanged(bool)), SLOT(update()));
    connect(m_panel, SIGNAL(manualSlidingChanged(bool)), SLOT(update()));
    m_requestAttentionProperty->connectNotifySignal(this, SLOT(update()));
    update();
}

HideModeController::~HideModeController()
{
}

void HideModeController::update()
{
    UQ_VAR(m_requestAttentionProperty->read());
    AutoHideMode mode = AutoHideMode(m_hideModeKey->getValue().toInt());

    delete m_controller;
    m_controller = 0;

    /* Do not use any hiding controller if the panel is either:
        - being slid manually
        - locked in place (using struts)
    */
    if (!m_panel->manualSliding() && !m_panel->useStrut()) {
        switch (mode) {
        case ManualHide:
            break;
        case AutoHide:
            m_controller = new AutohideController(m_panel);
            break;
        case IntelliHide:
            m_controller = new IntellihideController(m_panel);
            break;
        }
    }
}

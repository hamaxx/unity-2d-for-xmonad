/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Olivier Tilloy <olivier.tilloy@canonical.com>
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

#include "superkeymonitor.h"
#include "keymonitor.h"

#include "gconfitem-qml-wrapper.h"

SuperKeyMonitor::SuperKeyMonitor()
{
    m_left = new KeyMonitor(SUPER_L, this);
    m_right = new KeyMonitor(SUPER_R, this);

    m_enable_setting = new GConfItemQmlWrapper(this);
    m_enable_setting->setKey("/desktop/unity/launcher/super_key_enable");
    if (getEnableSettingValue()) {
        startMonitoring();
    }
    connect(m_enable_setting, SIGNAL(valueChanged()), SLOT(slotEnableSettingChanged()));
}

SuperKeyMonitor::~SuperKeyMonitor()
{
}

bool
SuperKeyMonitor::getEnableSettingValue() const
{
    QVariant value = m_enable_setting->getValue();
    if (value.isValid()) {
        return value.toBool();
    }
    else {
        /* The key is not set, assume true. */
        return true;
    }
}

void
SuperKeyMonitor::slotEnableSettingChanged()
{
    if (getEnableSettingValue()) {
        startMonitoring();
    }
    else {
        stopMonitoring();
    }
}

void
SuperKeyMonitor::startMonitoring()
{
    m_left->grabKey();
    m_right->grabKey();
}

void
SuperKeyMonitor::stopMonitoring()
{
    m_left->ungrabKey();
    m_right->ungrabKey();
}


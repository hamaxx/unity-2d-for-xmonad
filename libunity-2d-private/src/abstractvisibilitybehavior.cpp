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
#include "abstractvisibilitybehavior.h"

// Local

// libunity-2d

// Qt
#include <QWidget>

AbstractVisibilityBehavior::AbstractVisibilityBehavior(QWidget* panel)
: QObject(panel)
, m_panel(panel)
, m_visible(true)
{
}

AbstractVisibilityBehavior::~AbstractVisibilityBehavior()
{
}

bool AbstractVisibilityBehavior::visible() const
{
    return m_visible;
}

QWidget* AbstractVisibilityBehavior::panel() const
{
    return m_panel;
}

void AbstractVisibilityBehavior::setPanel(QWidget* panel)
{
    m_panel = panel;
    Q_EMIT panelChanged(m_panel);
}

#include "abstractvisibilitybehavior.moc"

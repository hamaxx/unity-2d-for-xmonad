/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Michał Sawicz <michal.sawicz@canonical.com>
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

#include "huddbus.h"
#include "hudadaptor.h"

// Local
#include <shelldeclarativeview.h>

// Qt
#include <QtDBus/QDBusConnection>
#include <QGraphicsObject>

HUDDBus::HUDDBus(ShellDeclarativeView* view, QObject* parent)
: QObject(parent)
, m_view(view)
{
    /* QML's propertyChanged signals are simple, they don't pass the property value */
    connect(m_view->rootObject(), SIGNAL(hudActiveChanged()), SLOT(onHudActiveChanged()));

    new HUDAdaptor(this);
}

bool
HUDDBus::active() const
{
    return m_view->rootObject()->property("hudActive").toBool();
}

void
HUDDBus::onHudActiveChanged()
{
    Q_EMIT activeChanged(active());
}

void
HUDDBus::setActive(bool hudActive)
{
    if (hudActive != active()) {
        m_view->rootObject()->setProperty("hudActive", hudActive);
        Q_EMIT activeChanged(hudActive);
    }
}

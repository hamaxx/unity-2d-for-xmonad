/*
 * Plasma applet to display DBus global menu
 *
 * Copyright 2009 Canonical Ltd.
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
#include "fdotask.h"

// Local
#include "x11embedcontainer.h"
#include "x11embeddelegate.h"

// uqpanel
#include <debug_p.h>

// Qt
#include <QEvent>

namespace SystemTray
{

FdoTask::FdoTask(WId id, QObject* parent)
: Task(parent)
, m_id(id)
, m_clientEmbedded(false)
, m_widget(0)
{
}

FdoTask::~FdoTask()
{
    taskDeleted(m_id);
    m_widget->deleteLater();
}

void FdoTask::createWidget()
{
    QMetaObject::invokeMethod(this, "setupXEmbedDelegate", Qt::QueuedConnection);
}

void FdoTask::setupXEmbedDelegate()
{
    if (m_widget) {
        return;
    }

    m_widget = new X11EmbedDelegate();

    connect(m_widget->container(), SIGNAL(clientIsEmbedded()),
        SLOT(slotClientEmbedded()));
    connect(m_widget->container(), SIGNAL(clientClosed()),
        SLOT(deleteLater()));
    connect(m_widget->container(), SIGNAL(error(QX11EmbedContainer::Error)),
        SLOT(deleteLater()));

    m_widget->container()->embedSystemTrayClient(m_id);
}

void FdoTask::slotClientEmbedded()
{
    m_clientEmbedded = true;
    widgetCreated(m_widget);
}




} // namespace

#include "fdotask.moc"

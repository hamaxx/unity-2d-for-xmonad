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
#include "gconnector.h"

// Local
#include <debug_p.h>

// Qt

GConnector::GConnector()
{
}

GConnector::~GConnector()
{
    disconnectAll();
}

void GConnector::connect(gpointer instance, const char* signal, GCallback handler, gpointer data)
{
    gulong id = g_signal_connect(instance, signal, handler, data);
    auto it = m_connections.find(instance);
    if (it == m_connections.end()) {
        g_object_weak_ref(G_OBJECT(instance),
            GWeakNotify(weakNotifyCB),
            reinterpret_cast<gpointer>(this));
        m_connections.insert(instance, ConnectionIdList() << id);
    } else {
        it.value() << id;
    }
}

void GConnector::disconnect(gpointer instance)
{
    auto it = m_connections.find(instance);
    UQ_RETURN_IF_FAIL(it != m_connections.end());
    disconnect(it);
}

void GConnector::disconnectAll()
{
    while (!m_connections.isEmpty()) {
        disconnect(m_connections.begin());
    }
}

void GConnector::disconnect(Connections::Iterator it)
{
    gpointer instance = it.key();
    Q_FOREACH(gulong id, it.value()) {
        g_signal_handler_disconnect(instance, id);
    }
    g_object_weak_unref(G_OBJECT(instance),
        GWeakNotify(weakNotifyCB),
        reinterpret_cast<gpointer>(this));
    m_connections.erase(it);
}

void GConnector::weakNotifyCB(GConnector* that, GObject* instance)
{
    that->m_connections.remove(instance);
}

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

// Qt

GConnector::GConnector(QObject* parent)
: QObject(parent)
{
}

GConnector::~GConnector()
{
    gdisconnectAll();
}

void GConnector::gconnect(gpointer instance, const char* signal, GCallback handler, gpointer data)
{
    Connection connection;
    connection.instance = instance;
    connection.id = g_signal_connect(instance, signal, handler, data);
    m_connections << connection;
}

void GConnector::gdisconnectAll()
{
    Q_FOREACH(const Connection& connection, m_connections) {
        g_signal_handler_disconnect(connection.instance, connection.id);
    }
    m_connections.clear();
}

#include "gconnector.moc"

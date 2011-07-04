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
#ifndef GCONNECTOR_H
#define GCONNECTOR_H

// Local

// Qt
#include <QHash>
#include <QList>

// GLib
#include <glib-object.h>

/**
 * An helper class to ensure GObject signals are correctly disconnected.
 */
class GConnector
{
public:
    GConnector();
    ~GConnector();

    /**
     * Connect a signal to a callback
     */
    void connect(gpointer instance, const char* signal, GCallback handler, gpointer data);

    /**
     * Disconnect from all signals of instance
     */
    void disconnect(gpointer instance);

    /**
     * Disconnect from all signals we connected to
     */
    void disconnectAll();

private:
    typedef QList<gulong> ConnectionIdList;
    typedef QHash<gpointer, ConnectionIdList> Connections;
    Connections m_connections;

    void disconnect(Connections::Iterator);

    static void weakNotifyCB(GConnector*, GObject*);
};

#endif /* GCONNECTOR_H */

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
#include <QObject>

// GLib
#include <glib-object.h>

/**
 * An helper class to ensure GObject signals are correctly disconnected.
 */
class GConnector : public QObject
{
    Q_OBJECT

    struct Connection {
        gpointer instance;
        gulong id;
    };
public:
    GConnector(QObject* parent=0);
    ~GConnector();

    void gconnect(gpointer instance, const char* signal, GCallback handler, gpointer data);
    void gdisconnectAll();

private:
    QList<Connection> m_connections;
};

#endif /* GCONNECTOR_H */

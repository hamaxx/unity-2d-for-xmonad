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

// Local
#include <debug_p.h>
#include <gconnector.h>

// Qt
#include <QtTest>

// GLib
#include <gio/gio.h>

class CallbackSpy {
public:
    CallbackSpy()
    : count(0)
    {}

    static void increase(GCancellable*, gpointer data)
    {
        CallbackSpy* spy = reinterpret_cast<CallbackSpy*>(data);
        ++spy->count;
    }

    int count;
};

class GConnectorTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void init()
    {
        g_type_init();
    }

    void testDisconnectAll()
    {
        // Create a cancellable object as it is easy to get it to emit a
        // signal.
        GCancellable* object = g_cancellable_new();

        CallbackSpy spy;
        GConnector connector;
        connector.connect(object, "cancelled", G_CALLBACK(CallbackSpy::increase), &spy);

        // Emit signal, check callback gets called
        g_cancellable_cancel(object);
        QCOMPARE(spy.count, 1);

        // Disconnect and emit signal, check callback does *not* get called
        connector.disconnectAll();
        g_cancellable_reset(object);
        g_cancellable_cancel(object);
        QCOMPARE(spy.count, 1);

        g_object_unref(object);
    }

    void testDeletedInstance()
    {
        GCancellable* object = g_cancellable_new();

        // Create a dummy connection
        CallbackSpy spy;
        GConnector connector;
        connector.connect(object, "cancelled", G_CALLBACK(CallbackSpy::increase), &spy);
        g_object_unref(object);

        // Should not crash because connection should have been removed
        connector.disconnectAll();
    }
};

QTEST_MAIN(GConnectorTest)

#include "gconnectortest.moc"

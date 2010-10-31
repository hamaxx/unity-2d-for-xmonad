/*
 * Copyright (C) 2010 Canonical, Ltd.
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

#ifndef DEVICE_H
#define DEVICE_H

#include <gio/gio.h>

#include "launcheritem.h"

#include <QObject>
#include <QString>
#include <QMetaType>

class Device : public LauncherItem
{
    Q_OBJECT

public:
    Device();
    Device(const Device& other);
    ~Device();

    /* getters */
    virtual bool active() const;
    virtual bool running() const;
    virtual bool urgent() const;
    virtual QString name() const;
    virtual QString icon() const;
    virtual bool launching() const;

    /* methods */
    Q_INVOKABLE virtual void activate();
    Q_INVOKABLE GVolume* getVolume();
    Q_INVOKABLE void setVolume(GVolume* volume);
    Q_INVOKABLE void open();
    Q_INVOKABLE void eject();

private:
    GVolume* m_volume;

    static void onVolumeMounted(GVolume* volume, GAsyncResult* res);
    static void onVolumeEjected(GVolume* volume, GAsyncResult* res);
};

Q_DECLARE_METATYPE(Device*)

#endif // DEVICE_H


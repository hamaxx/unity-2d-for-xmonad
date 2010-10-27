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
#include <QObject>
#include <QString>
#include <QMetaType>

class QDevice : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY nameChanged)

public:
    QDevice(QObject *parent = 0);
    QDevice(const QDevice& other);
    ~QDevice();

    /* getters */
    QString name() const;

    /* methods */
    void setVolume(GVolume* volume);
    void open();
    void eject();

signals:
    void nameChanged(QString);

private:
    GVolume *m_volume;

    static void onVolumeMounted(GVolume* volume, GAsyncResult* res);
    static void onVolumeEjected(GVolume* volume, GAsyncResult* res);
};

Q_DECLARE_METATYPE(QDevice*)

#endif // DEVICE_H

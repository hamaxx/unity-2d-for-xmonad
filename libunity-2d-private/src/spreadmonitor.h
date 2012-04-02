/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Ugo Riboni <ugo.riboni@canonical.com>
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

#ifndef SPREADMONITOR_H
#define SPREADMONITOR_H

// QT
#include <QObject>
#include <QMetaType>

// local
#include "abstractdbusservicemonitor.h"

class SpreadMonitor : public AbstractDBusServiceMonitor
{
    Q_OBJECT
    Q_PROPERTY(bool shown READ shown NOTIFY shownChanged)

public:
    explicit SpreadMonitor(QObject *parent = 0);
    bool shown() const;

protected Q_SLOTS:
    void onServiceAvailableChanged(bool available);

Q_SIGNALS:
    void shownChanged(bool visible);

private Q_SLOTS:
    void internalSetShown(bool shown);

private:
    bool dbusShown() const;

    bool m_shown;
};

Q_DECLARE_METATYPE(SpreadMonitor*)

#endif // SPREADMONITOR_H

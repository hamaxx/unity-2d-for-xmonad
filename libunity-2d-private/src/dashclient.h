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
#ifndef DASHCLIENT_H
#define DASHCLIENT_H

// Local

// Qt
#include <QObject>
#include <QSize>

class QDBusInterface;

/**
 * Monitors the dash and provide a single point of entry to its status
 * TODO: rename to reflect transition to Shell.
 */
class DashClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool alwaysFullScreen READ alwaysFullScreen NOTIFY alwaysFullScreenChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool hudActive READ hudActive WRITE setHudActive NOTIFY hudActiveChanged)

public:
    static DashClient* instance();

    bool active() const;
    void setActive(bool active);
    bool hudActive() const;
    void setHudActive(bool active);

    bool alwaysFullScreen() const;

Q_SIGNALS:
    void activeChanged(bool);
    void alwaysFullScreenChanged();
    void hudActiveChanged(bool);

private Q_SLOTS:
    void connectToDash();
    void slotDashActiveChanged(bool);
    void slotHudActiveChanged(bool);
    void slotAlwaysFullScreenChanged(bool);

private:
    DashClient(QObject* parent = 0);

    QDBusInterface* m_dashDbusIface;
    bool m_dashActive;
    bool m_hudActive;
    bool m_alwaysFullScreen;
};

#endif /* DASHCLIENT_H */

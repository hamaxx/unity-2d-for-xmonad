/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 *  Michał Sawicz <michal.sawicz@canonical.com>
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
#ifndef HUDCLIENT_H
#define HUDCLIENT_H

// Local

// Qt
#include <QObject>
#include <QSize>

class QDBusInterface;

/**
 * Monitor the HUD and provide a single point of entry to its status
 */
class HUDClient : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(int screen READ screen NOTIFY screenChanged)

public:
    static HUDClient* instance();

    bool active() const;
    void setActive(bool active);

    int screen() const;

    bool activeInScreen(int screen) const;

Q_SIGNALS:
    void activeChanged(bool);
    void screenChanged(int);

private Q_SLOTS:
    void connectToHud();
    void slotActiveChanged(bool);
    void slotScreenChanged(int);

private:
    HUDClient(QObject* parent = 0);

    QDBusInterface* m_hudDbusIface;
    bool m_active;
    int m_screen;
};

#endif /* HUDCLIENT_H */

/*
 * Copyright (C) 2011 Canonical, Ltd.
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

#ifndef HUDDBus_H
#define HUDDBus_H

#include <QtCore/QObject>
#include <QtDBus/QDBusContext>

class ShellManager;

/**
 * DBus interface for the HUD.
 */
class HUDDBus : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(int screen READ screen NOTIFY screenChanged)

public:
    HUDDBus(ShellManager* manager, QObject* parent=0);

    bool active() const;
    void setActive(bool active);
    int screen() const;

Q_SIGNALS:
    void activeChanged(bool);
    void screenChanged(int);

private Q_SLOTS:
    void onHudActiveChanged();

private:
    ShellManager* m_manager;
};

#endif // HudDBus_H


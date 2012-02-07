/*
 * Copyright (C) 2012 Canonical, Ltd.
 *
 * Authors:
 *  Gerry Boland <gerry.boland@canonical.com>
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

#ifndef HUDDBUS_H
#define HUDDBUS_H

#include <QtCore/QObject>
#include <QtDBus/QDBusContext>

class ShellDeclarativeView;

/**
 * DBus interface for the hud.
 *
 * Note: Methods from this class should not be called from within the Hud:
 * some of them may rely on the call coming from DBus.
 */
class HudDBus : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)

public:
    HudDBus(ShellDeclarativeView* view, QObject* parent=0);
    ~HudDBus();

    bool connectToBus();

    bool active() const;
    void setActive(bool active);

Q_SIGNALS:
    void activeChanged(bool);

private:
    ShellDeclarativeView* m_view;
};

#endif // HUDDBUS_H

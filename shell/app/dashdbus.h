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

#ifndef DashDBus_H
#define DashDBus_H

#include <QtCore/QObject>
#include <QtDBus/QDBusContext>

class ShellManager;

/**
 * DBus interface for the dash.
 *
 * Note: Methods from this class should not be called from within the Dash:
 * some of them may rely on the call coming from DBus.
 */
class DashDBus : public QObject, protected QDBusContext
{
    Q_OBJECT
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool alwaysFullScreen READ alwaysFullScreen NOTIFY alwaysFullScreenChanged)
    Q_PROPERTY(QString activeLens READ activeLens WRITE setActiveLens NOTIFY activeLensChanged)
    Q_PROPERTY(int screen READ screen NOTIFY screenChanged)

public:
    DashDBus(ShellManager* manager, QObject* parent=0);

    bool active() const;
    void setActive(bool active);
    bool alwaysFullScreen() const;
    QString activeLens() const;
    void setActiveLens(QString activeLens);
    int screen() const;

public Q_SLOTS:
    Q_NOREPLY void activateHome();
    Q_NOREPLY void activateLens(const QString& lensId);

Q_SIGNALS:
    void activeChanged(bool);
    void alwaysFullScreenChanged(bool);
    void activeLensChanged(QString);
    void screenChanged(int);

private:
    ShellManager* m_manager;
};

#endif // DashDBus_H


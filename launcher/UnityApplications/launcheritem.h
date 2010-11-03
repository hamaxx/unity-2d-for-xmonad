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

#ifndef LAUNCHERITEM_H
#define LAUNCHERITEM_H

#include <QObject>
#include <QString>

#include "launchermenu.h"

class LauncherItem : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool active READ active NOTIFY activeChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(bool urgent READ urgent NOTIFY urgentChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(bool launching READ launching NOTIFY launchingChanged)

public:
    LauncherItem();
    ~LauncherItem();

    /* getters */
    virtual bool active() const = 0;
    virtual bool running() const = 0;
    virtual bool urgent() const = 0;
    virtual QString name() const = 0;
    virtual QString icon() const = 0;
    virtual bool launching() const = 0;

    /* methods */
    Q_INVOKABLE virtual void activate() = 0;
    Q_INVOKABLE void showTooltip(int y);
    Q_INVOKABLE void showMenu();
    Q_INVOKABLE virtual void reallyShowMenu() = 0;
    Q_INVOKABLE void hideMenu(bool force = false);

protected:
    LauncherContextualMenu* m_menu;

    bool eventFilter(QObject* obj, QEvent* event);

signals:
    void activeChanged(bool);
    void runningChanged(bool);
    void urgentChanged(bool);
    void nameChanged(QString);
    void iconChanged(QString);
    void launchingChanged(bool);
};

#endif // LAUNCHERITEM_H


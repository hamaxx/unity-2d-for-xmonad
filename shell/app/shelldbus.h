/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Ugo Riboni <ugo.riboni@canonical.com>
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

#ifndef ShellDBus_H
#define ShellDBus_H

#include <QtCore/QObject>
#include <QtDBus/QDBusContext>

class ShellDeclarativeView;

/**
 * DBus interface for the shell.
 */
class ShellDBus : public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    ShellDBus(ShellDeclarativeView* view, QObject* parent=0);
    ~ShellDBus();

    bool connectToBus();

private:
    ShellDeclarativeView* m_view;
};

#endif // ShellDBus_H


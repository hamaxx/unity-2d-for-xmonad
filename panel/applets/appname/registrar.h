/*
 * Plasma applet to display application window menus
 *
 * Copyright 2010 Canonical Ltd.
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

#ifndef REGISTRAR_H
#define REGISTRAR_H

// Qt
#include <QDBusContext>
#include <QDBusObjectPath>
#include <QObject>
#include <QWidget> // For WId

class QDBusObjectPath;
class QDBusServiceWatcher;
class QMenu;

struct MenuInfo
{
    MenuInfo()
    : winId(0)
    , path("/")
    {}

    uint winId;
    QString service;
    QDBusObjectPath path;
};
Q_DECLARE_METATYPE(MenuInfo)

typedef QList<MenuInfo> MenuInfoList;
Q_DECLARE_METATYPE(MenuInfoList)

class Registrar : public QObject, protected QDBusContext
{
    Q_OBJECT

public:
    /* The registrar is a singleton shared between all instances of MenuBarWidget. */
    static Registrar* instance();

    bool connectToBus(const QString& service = QString(), const QString& objectPath = QString());

Q_SIGNALS:
    void WindowRegistered(WId wid, const QString& service, const QDBusObjectPath&);
    void WindowUnregistered(WId wid);

public Q_SLOTS:
    Q_NOREPLY void RegisterWindow(WId wid, const QDBusObjectPath& menuObjectPath);
    Q_NOREPLY void UnregisterWindow(WId wid);
    QString GetMenuForWindow(WId wid, QDBusObjectPath& menuObjectPath);
    MenuInfoList GetMenus();

private Q_SLOTS:
    void slotServiceUnregistered(const QString& service);

private:
    explicit Registrar();
    Q_DISABLE_COPY(Registrar)
    ~Registrar();

    QDBusServiceWatcher* mServiceWatcher;
    typedef QHash<WId, MenuInfo> MenuInfoDb;
    MenuInfoDb mDb;
    QString mService;
};

#endif /* REGISTRAR_H */

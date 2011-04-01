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

// Self
#include "registrar.h"

// Qt
#include <QApplication>
#include <QDBusMessage>
#include <QDBusObjectPath>
#include <QDBusServiceWatcher>

// Local
#include "registraradaptor.h"

static const char* DBUS_SERVICE = "com.canonical.AppMenu.Registrar";
static const char* DBUS_OBJECT_PATH = "/com/canonical/AppMenu/Registrar";

// Marshalling code for MenuInfo
QDBusArgument& operator<<(QDBusArgument& argument, const MenuInfo& info)
{
    argument.beginStructure();
    argument << info.winId << info.service << info.path;
    argument.endStructure();
    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, MenuInfo& info)
{
    argument.beginStructure();
    argument >> info.winId >> info.service >> info.path;
    argument.endStructure();
    return argument;
}

Registrar::Registrar()
: QObject()
, mServiceWatcher(new QDBusServiceWatcher(this))
{
    qDBusRegisterMetaType<MenuInfo>();
    qDBusRegisterMetaType<MenuInfoList>();
    mServiceWatcher->setConnection(QDBusConnection::sessionBus());
    mServiceWatcher->setWatchMode(QDBusServiceWatcher::WatchForUnregistration);
    connect(mServiceWatcher, SIGNAL(serviceUnregistered(const QString&)), SLOT(slotServiceUnregistered(const QString&)));
}

Registrar::~Registrar()
{
    QDBusConnection::sessionBus().unregisterService(mService);
}

Registrar* Registrar::instance()
{
    static Registrar* singleton = new Registrar();
    return singleton;
}

bool Registrar::connectToBus(const QString& _service, const QString& _path)
{
    mService = _service.isEmpty() ? DBUS_SERVICE : _service;
    QString path = _path.isEmpty() ? DBUS_OBJECT_PATH : _path;

    bool ok = QDBusConnection::sessionBus().registerService(mService);
    if (!ok) {
        return false;
    }
    new RegistrarAdaptor(this);
    QDBusConnection::sessionBus().registerObject(path, this);

    return true;
}

void Registrar::RegisterWindow(WId wid, const QDBusObjectPath& menuObjectPath)
{
    MenuInfo info;
    info.winId = wid;
    info.service = message().service();
    info.path = menuObjectPath;
    mDb.insert(wid, info);
    mServiceWatcher->addWatchedService(info.service);
    WindowRegistered(wid, info.service, info.path);
}

void Registrar::UnregisterWindow(WId wid)
{
    mDb.remove(wid);
    WindowUnregistered(wid);
}

QString Registrar::GetMenuForWindow(WId winId, QDBusObjectPath& menuObjectPath)
{
    MenuInfo info = mDb.value(winId);
    QString service = info.service;
    menuObjectPath = info.path;
    return service;
}

MenuInfoList Registrar::GetMenus()
{
    return mDb.values();
}

void Registrar::slotServiceUnregistered(const QString& service)
{
    MenuInfoDb::Iterator
        it = mDb.begin(),
        end = mDb.end();
    for (;it != end;) {
        if (it.value().service == service) {
            WId id = it.key();
            it = mDb.erase(it);
            WindowUnregistered(id);
        } else {
            ++it;
        }
    }
    mServiceWatcher->removeWatchedService(service);
}

#include "registrar.moc"

/*
 * This file is part of unity-2d
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

#include "sniitem.h"

// Local
#include "debug_p.h"

// dbusmenu-qt
#include <dbusmenuimporter.h>

// Qt
#include <QAction>
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QMenuBar>
#include <QVariant>

static const char* SNI_IFACE = "org.kde.StatusNotifierItem";
static const char* FDO_PROPERTIES_IFACE = "org.freedesktop.DBus.Properties";

SNIItem::SNIItem(const QString& service, const QString& path, QMenuBar* menuBar)
: QObject(menuBar)
, m_iface(service, path, SNI_IFACE)
, m_menuBar(menuBar)
, m_action(new QAction(this))
{
    m_menuBar->setNativeMenuBar(false);
    m_menuBar->addAction(m_action);
    updateFromDBus();
}

void SNIItem::updateFromDBus()
{
    QDBusMessage call = QDBusMessage::createMethodCall(m_iface.service(), m_iface.path(), FDO_PROPERTIES_IFACE, "GetAll");
    call.setArguments(QVariantList() << QString(SNI_IFACE));
    QDBusPendingCall reply = m_iface.connection().asyncCall(call);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(slotPropertiesReceived(QDBusPendingCallWatcher*)));
}

void SNIItem::slotPropertiesReceived(QDBusPendingCallWatcher* watcher)
{
    watcher->deleteLater();
    QDBusPendingReply<QVariantMap> reply = *watcher;
    if (!reply.isError()) {
        updateFromProperties(reply.value());
    } else {
        UQ_WARNING << "GetAll failed:" << reply.error();
    }
}

void SNIItem::updateFromProperties(const QVariantMap& map)
{
    QVariant value;
    value = map.value("IconName");
    if (value.isValid()) {
        QIcon icon = QIcon::fromTheme(value.toString());
        m_action->setIcon(icon);
    }
    value = map.value("Menu");
    if (value.isValid()) {
        QDBusObjectPath path = value.value<QDBusObjectPath>();
        m_importer.reset(new DBusMenuImporter(m_iface.service(), path.path()));
        m_action->setMenu(m_importer->menu());
    }
}

#include "sniitem.moc"

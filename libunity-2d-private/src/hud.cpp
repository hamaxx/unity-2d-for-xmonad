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

// Self
#include "hud.h"

// std
#include <stdexcept>

// Qt
#include <QDebug>
#include <QDateTime>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusServiceWatcher>

static const char* HUD_DBUS_SERVICE = "com.canonical.hud";
static const char* HUD_DBUS_PATH = "/com/canonical/hud";

Hud::Hud(QObject *parent) :
    QAbstractListModel(parent),
    m_connected(false),
    m_searchQuery(QString())
{
    QHash<int, QByteArray> names;
    names[QueryIdRole] = "queryId";
    names[FormattedTextRole] = "formattedText";
    names[IconNameRole] = "iconName";
    names[ItemIconRole] = "itemIcon";
    names[CompletionTextRole] = "completionText";
    names[ShortcutRole] = "shortcut";
    setRoleNames(names);

    QDBusConnectionInterface* sessionBusIFace = QDBusConnection::sessionBus().interface();
    QDBusReply<bool> reply = sessionBusIFace->isServiceRegistered(HUD_DBUS_SERVICE);
    if (!reply.isValid() || !reply.value()) {
        qDebug() << "HUD service com.canonical.hud is NOT registered";
    }

    m_unityHud = new unity::hud::Hud(HUD_DBUS_SERVICE, HUD_DBUS_PATH);

    m_unityHud->target.changed.connect(sigc::mem_fun(this, &Hud::onTargetChanged));
    m_unityHud->connected.changed.connect(sigc::mem_fun(this, &Hud::onConnectedChanged));
    m_unityHud->queries_updated.connect(sigc::mem_fun(this, &Hud::onQueriesUpdated));
}

Hud::~Hud()
{
    delete m_unityHud;
}

int Hud::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    if (m_searchQuery.isEmpty()) {
        return 0;
    }
    return m_unityHudQueries.size();
}

QVariant Hud::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || m_searchQuery.isEmpty()) {
        return QVariant();
    }

    std::shared_ptr<unity::hud::Query> query;

    try {
        query = m_unityHudQueries.at(index.row());
    } catch (std::out_of_range) {
        qDebug() << "HUD query id invalid";
        return QVariant();
    }

    switch(role) {
    case Hud::QueryIdRole:
        return index.row();
    case Hud::FormattedTextRole:
        return QString::fromStdString(query->formatted_text);
    case Hud::IconNameRole:
        return QString::fromStdString(query->icon_name);
    case Hud::ItemIconRole:
        return QString::fromStdString(query->item_icon);
    case Hud::CompletionTextRole:
        return QString::fromStdString(query->completion_text);
    case Hud::ShortcutRole:
        return QString::fromStdString(query->shortcut);
    /* TODO (?) Hud returns more information in a "key" struct containing
     * lower-level information than we seem not to need right now. */
    default:
        return QVariant();
    }
}

QString Hud::searchQuery() const
{
    return m_searchQuery;
}

void Hud::setSearchQuery(const QString& searchQuery)
{
    if (searchQuery != m_searchQuery) {
        m_searchQuery = searchQuery;
        m_unityHud->RequestQuery(m_searchQuery.toStdString());
        Q_EMIT beginResetModel();
        Q_EMIT searchQueryChanged();
    }
}

void Hud::executeQuery(const int id) const
{
    std::shared_ptr<unity::hud::Query> query;

    try {
        query = m_unityHudQueries.at(id);
    } catch (std::out_of_range) {
        qDebug() << "HUD query id invalid";
        return;
    }

    m_unityHud->ExecuteQuery(query,
                             QDateTime::currentDateTime().toTime_t());
}

void Hud::executeQueryBySearch(const QString& searchQuery) const
{
    m_unityHud->ExecuteQueryBySearch(searchQuery.toStdString(),
                                     QDateTime::currentDateTime().toTime_t());
}

void Hud::closeQuery()
{
    m_unityHud->CloseQuery();
}

void Hud::onQueriesUpdated(const unity::hud::Hud::Queries queries)
{
    m_unityHudQueries = queries;
    Q_EMIT endResetModel();
}

QString Hud::target() const {
    return QString::fromStdString(m_unityHud->target);
}

bool Hud::connected() const{
    return m_connected;
}

void Hud::onConnectedChanged(const bool connected) {
    m_connected = connected;
    Q_EMIT connectedChanged();
}

void Hud::onTargetChanged(const std::string target) {
    Q_EMIT targetChanged();
}

#include "hud.moc"

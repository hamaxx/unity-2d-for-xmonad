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

// Local
#include <debug_p.h>

// Qt
#include <QDateTime>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDBusInterface>
#include <QDBusServiceWatcher>

static const char* HUD_DBUS_SERVICE = "com.canonical.hud";
static const char* HUD_DBUS_PATH = "/com/canonical/hud";

Hud::Hud(QObject *parent) :
    QAbstractListModel(parent),
    m_connected(false)
{
    QHash<int, QByteArray> names;
    names[ResultIdRole] = "resultId";
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
    m_unityHud->queries_updated.connect(sigc::mem_fun(this, &Hud::onResultsUpdated));
}

Hud::~Hud()
{
    delete m_unityHud;
}

int Hud::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    if (m_searchText.isEmpty()) {
        return 0;
    }
    return m_unityHudResults.size();
}

QVariant Hud::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || m_searchText.isEmpty()) {
        return QVariant();
    }

    std::shared_ptr<unity::hud::Query> result;

    try {
        result = m_unityHudResults.at(index.row());
    } catch (std::out_of_range) {
        UQ_DEBUG << "HUD query id invalid";
        return QVariant();
    }

    switch(role) {
    case Hud::ResultIdRole:
        return index.row();
    case Hud::FormattedTextRole:
        return QString::fromStdString(result->formatted_text);
    case Hud::IconNameRole:
        return QString::fromStdString(result->icon_name);
    case Hud::ItemIconRole:
        return QString::fromStdString(result->item_icon);
    case Hud::CompletionTextRole:
        return QString::fromStdString(result->completion_text);
    case Hud::ShortcutRole:
        return QString::fromStdString(result->shortcut);
    /* TODO (?) Hud returns more information in a "key" struct containing
     * lower-level information than we seem not to need right now. */
    default:
        return QVariant();
    }
}

QString Hud::searchText() const
{
    return m_searchText;
}

void Hud::setSearchText(const QString& searchText)
{
    if (searchText != m_searchText) {
        m_searchText = searchText;
        m_unityHud->RequestQuery(m_searchText.toStdString());
        beginResetModel();
        Q_EMIT searchTextChanged();
    }
}

void Hud::executeResult(const int id) const
{
    std::shared_ptr<unity::hud::Query> result;

    try {
        result = m_unityHudResults.at(id);
    } catch (std::out_of_range) {
        UQ_DEBUG << "HUD query id invalid";
        return;
    }

    m_unityHud->ExecuteQuery(result,
                             QDateTime::currentDateTime().toTime_t());
}

void Hud::executeResultBySearch(const QString& searchQuery) const
{
    m_unityHud->ExecuteQueryBySearch(searchQuery.toStdString(),
                                     QDateTime::currentDateTime().toTime_t());
}

void Hud::endSearch()
{
    m_unityHud->CloseQuery();
}

void Hud::onResultsUpdated(const unity::hud::Hud::Queries results)
{
    m_unityHudResults = results;
    endResetModel();
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

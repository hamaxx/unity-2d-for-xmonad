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

#ifndef HUD_H
#define HUD_H

// Qt
#include <QAbstractListModel>

// libunity-core
#include <UnityCore/Hud.h>

class Hud : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString target READ target NOTIFY targetChanged)
    Q_PROPERTY(bool connected READ connected NOTIFY connectedChanged)
    Q_PROPERTY(QString searchQuery READ searchQuery WRITE setSearchQuery NOTIFY searchQueryChanged)

public:
    explicit Hud(QObject *parent = 0);
    ~Hud();

    enum Roles {
        ResultIdRole = Qt::UserRole+1,
        FormattedTextRole,
        PlainTextRole,
        IconNameRole,
        ItemIconRole,
        CompletionTextRole,
        ShortcutRole
    };

    /* getters */
    QString target() const;
    QString searchQuery() const;
    bool connected() const;

    /* setters */
    void setSearchQuery(const QString&);

    QVariant data(int role) const;
    QHash<int, QByteArray> roleNames() const;

    /* Implementation of virtual methods from QAbstractListModel */
    QVariant data(const QModelIndex& index, int role = Hud::ResultIdRole) const;
    int rowCount(const QModelIndex& parent = QModelIndex()) const;

    Q_INVOKABLE void executeResult(const int) const;
    Q_INVOKABLE void executeResultBySearch(const QString&) const;
    Q_INVOKABLE void endSearch();

Q_SIGNALS:
    void searchQueryChanged();
    void targetChanged();
    void connectedChanged();

private Q_SLOTS:
    void onTargetChanged(const std::string);
    void onConnectedChanged(const bool);
    void onResultsUpdated(const unity::hud::Hud::Queries);

private:
    bool m_connected;
    QString m_searchQuery;
    unity::hud::Hud* m_unityHud;
    unity::hud::Hud::Queries m_unityHudResults; //doubly-ended queue of 'Query's.
};

Q_DECLARE_METATYPE(Hud*)

#endif // HUD_H

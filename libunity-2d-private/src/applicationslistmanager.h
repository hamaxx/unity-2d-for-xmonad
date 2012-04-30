/*
 * Copyright (C) 2012 Canonical, Ltd.
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

#ifndef APPLICATIONSLISTMODEL_H
#define APPLICATIONSLISTMODEL_H

#include <QObject>
#include <QDBusContext>

#include <QSet>

class ApplicationsList;

class ApplicationsListManager : public QObject, protected QDBusContext
{
    Q_OBJECT
    friend class ApplicationsListDBUS;
    friend class ApplicationsList;

public:
    static ApplicationsListManager *instance();

    void addList(ApplicationsList *list);
    void removeList(ApplicationsList *list);

    Q_INVOKABLE void insertFavoriteApplication(const QString& desktop_file);
    Q_INVOKABLE void insertWebFavorite(const QUrl& url);

private Q_SLOTS:
    void onRemoteEntryUpdated(QString applicationURI,
                              QMap<QString, QVariant> properties);

private:
    ApplicationsListManager();

    QSet<ApplicationsList*> m_lists;
};

#endif // APPLICATIONSLISTMODEL_H

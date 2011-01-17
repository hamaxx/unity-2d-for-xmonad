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

#ifndef SNIITEM_H
#define SNIITEM_H

// Qt
#include <QDBusInterface>
#include <QObject>

class DBusMenuImporter;

class QAction;
class QDBusPendingCallWatcher;
class QMenuBar;

class SNIItem : public QObject
{
    Q_OBJECT
public:
    SNIItem(const QString& service, const QString& path, QMenuBar* menuBar);

private Q_SLOTS:
    void slotPropertiesReceived(QDBusPendingCallWatcher*);

private:
    QDBusInterface m_iface;
    QMenuBar* m_menuBar;
    QAction* m_action;
    QScopedPointer<DBusMenuImporter> m_importer;
    void updateFromDBus();
    void updateFromProperties(const QVariantMap&);
};

#endif // SNIITEM_H

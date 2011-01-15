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

#ifndef INDICATORSERVICEMANAGER_H
#define INDICATORSERVICEMANAGER_H

// Local

// Qt
#include <QObject>

class QAction;
class QDBusPendingCallWatcher;

class IndicatorServiceManager : public QObject
{
    Q_OBJECT
public:
    IndicatorServiceManager(const char* name, uint version, QObject* parent=0);
    ~IndicatorServiceManager();

private Q_SLOTS:
    void slotServiceOwnerChanged(const QString& name, const QString& oldOwner, const QString& newOwner);
    void slotWatchFinished(QDBusPendingCallWatcher*);

private:
    Q_DISABLE_COPY(IndicatorServiceManager)
    QString m_serviceName;
    uint m_serviceVersion;

    void connectToService();
    void watchService();
    void unwatchService();
};

#endif /* INDICATORSERVICEMANAGER_H */

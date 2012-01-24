/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
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

#ifndef GNOMESESSIONCLIENT_H
#define GNOMESESSIONCLIENT_H

// Local

// Qt
#include <QObject>

class QDBusPendingCallWatcher;

struct GnomeSessionClientPrivate;
/**
 * This class makes it possible for an application to register with GNOME
 * SessionManager. This is necessary for the SessionManager to be able to
 * restart the application.
 */
class GnomeSessionClient : public QObject
{
Q_OBJECT
public:
    /**
     * @param applicationId: absolute path to the application desktop file
     */
    GnomeSessionClient(const QString& applicationId, QObject* parent = 0);
    ~GnomeSessionClient();

    void connectToSessionManager();

private Q_SLOTS:
    void slotRegisterClientFinished(QDBusPendingCallWatcher* watcher);
    void stop();
    void queryEndSession();
    void endSession();
    void waitForEndSession();

private:
    GnomeSessionClientPrivate* const d;
};

#endif /* GNOMESESSIONCLIENT_H */

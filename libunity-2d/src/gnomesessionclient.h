/*
 * This file is part of unity-2d
 *
 * Copyright 2011 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
 */
#ifndef GNOMESESSIONCLIENT_H
#define GNOMESESSIONCLIENT_H

// Local

// Qt
#include <QObject>

class QDBusPendingCallWatcher;

struct GnomeSessionClientPrivate;
class GnomeSessionClient : public QObject
{
Q_OBJECT
public:
    GnomeSessionClient(const QString& applicationId, QObject* parent = 0);
    ~GnomeSessionClient();

    void connectToSessionManager();

private Q_SLOTS:
    void slotRegisterClientFinished(QDBusPendingCallWatcher* watcher);
    void stop();
    void queryEndSession();
    void endSession();

private:
    GnomeSessionClientPrivate* const d;
};

#endif /* GNOMESESSIONCLIENT_H */

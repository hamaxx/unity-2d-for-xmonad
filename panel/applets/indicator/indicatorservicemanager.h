/*
 * This file is part of unity-qt
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
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

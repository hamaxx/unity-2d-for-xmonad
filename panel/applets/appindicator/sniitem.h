/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * License: GPL v3
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

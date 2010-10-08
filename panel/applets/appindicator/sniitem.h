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
#ifndef SNIITEM_H
#define SNIITEM_H

// Qt
#include <QDBusInterface>
#include <QWidget>

class QDBusPendingCallWatcher;

class SNIItem : public QWidget
{
    Q_OBJECT
public:
    SNIItem(const QString& service, const QString& path, QWidget* parent);

protected:
    virtual void paintEvent(QPaintEvent*);

private Q_SLOTS:
    void slotPropertiesReceived(QDBusPendingCallWatcher*);

private:
    QDBusInterface m_iface;
    void updateFromDBus();
    void updateFromProperties(const QVariantMap&);
};

#endif // SNIITEM_H

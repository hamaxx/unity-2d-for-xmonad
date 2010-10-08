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
#include "sniitem.h"

// Local
#include "debug_p.h"

// Qt
#include <QDBusPendingCallWatcher>
#include <QDBusPendingReply>
#include <QVariant>

static const char* SNI_IFACE = "org.kde.StatusNotifierItem";
static const char* FDO_PROPERTIES_IFACE = "org.freedesktop.DBus.Properties";

SNIItem::SNIItem(const QString& service, const QString& path, QWidget* parent)
: QWidget(parent)
, m_iface(service, path, SNI_IFACE)
{
    updateFromDBus();
}

void SNIItem::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);
}

void SNIItem::updateFromDBus()
{
    QDBusMessage call = QDBusMessage::createMethodCall(m_iface.service(), m_iface.path(), FDO_PROPERTIES_IFACE, "GetAll");
    call.setArguments(QVariantList() << QString(SNI_IFACE));
    QDBusPendingCall reply = m_iface.connection().asyncCall(call);
    QDBusPendingCallWatcher* watcher = new QDBusPendingCallWatcher(reply, this);

    connect(watcher, SIGNAL(finished(QDBusPendingCallWatcher*)), SLOT(slotPropertiesReceived(QDBusPendingCallWatcher*)));
}

void SNIItem::slotPropertiesReceived(QDBusPendingCallWatcher* watcher)
{
    watcher->deleteLater();
    QDBusPendingReply<QVariantMap> reply = *watcher;
    if (!reply.isError()) {
        updateFromProperties(reply.value());
    } else {
        UQ_WARNING << "GetAll failed:" << reply.error();
    }
}

void SNIItem::updateFromProperties(const QVariantMap& map)
{
    UQ_VAR(map);
}

#include "sniitem.moc"
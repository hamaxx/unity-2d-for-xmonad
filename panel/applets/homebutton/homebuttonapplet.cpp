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
// Self
#include "homebuttonapplet.h"

// Local
#include <debug_p.h>

// Qt
#include <QHBoxLayout>
#include <QDBusInterface>
#include <QDBusPendingCall>

static const char* DBUS_SERVICE = "com.canonical.UnityQt";
static const char* DBUS_PATH = "/dash";
static const char* DBUS_IFACE = "org.freedesktop.DBus.Properties";

// FIXME: Hardcoded and part of unity package, not unity-asset-pool
static const char* ICON_NAME = "/usr/share/unity/bfb.png";

HomeButtonApplet::HomeButtonApplet()
: m_button(new QToolButton)
{
    m_button->setAutoRaise(true);
    m_button->setIcon(QIcon(ICON_NAME));
    m_button->setIconSize(QSize(20, 22));
    connect(m_button, SIGNAL(clicked()), SLOT(slotButtonClicked()));

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_button);
}

void HomeButtonApplet::slotButtonClicked()
{
    QDBusInterface iface(DBUS_SERVICE, DBUS_PATH, DBUS_IFACE);

    /* Retrieve the value of boolean local.DashDeclarativeView.active */
    QDBusMessage reply = iface.call(QDBus::Block, "Get", "local.DashDeclarativeView", "active");
    QVariant rawResult = reply.arguments()[0];
    QDBusVariant dbusVariant = qvariant_cast<QDBusVariant>(rawResult);
    bool dashActive = dbusVariant.variant().toBool();

    /* Inverse the value of boolean local.DashDeclarativeView.active */
    iface.call(QDBus::Block, "Set", "local.DashDeclarativeView", "active", qVariantFromValue(QDBusVariant(!dashActive)));
}

#include "homebuttonapplet.moc"

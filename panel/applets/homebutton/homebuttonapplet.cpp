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

HomeButtonApplet::HomeButtonApplet()
: m_button(new QToolButton)
{
    m_button->setAutoRaise(true);
    QIcon::setThemeName("unity-icon-theme");
    /* FIXME: Even though the size is set properly the wrong png is loaded.
              unity-icon-theme has 2 visually different distributor-logo icons,
              one 22x22 and another 24x24 (the one we want).
    */
    m_button->setIconSize(QSize(24, 24));
    m_button->setIcon(QIcon::fromTheme("distributor-logo"));
    m_button->setCheckable(true);
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

    /* Re-synchronise the dash's state with the button's checked state, just in case. */
    m_button->setChecked(!dashActive);
}

#include "homebuttonapplet.moc"

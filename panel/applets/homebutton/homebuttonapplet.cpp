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

static const char* DBUS_SERVICE = "org.freedesktop.Notifications";
static const char* DBUS_PATH = "/org/freedesktop/Notifications";
static const char* DBUS_IFACE = "org.freedesktop.Notifications";

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
    iface.asyncCall("Notify", "app", QVariant(uint(0)), "", "Home Button", "Clicked the home button", QStringList(), QVariantMap(), -1);
}

#include "homebuttonapplet.moc"

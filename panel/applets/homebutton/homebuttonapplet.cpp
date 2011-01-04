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
static const char* DBUS_IFACE = "local.DashDeclarativeView";

HomeButtonApplet::HomeButtonApplet()
: m_button(new QToolButton), m_dash_iface(NULL)
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

    m_button->setStyleSheet(
            "QToolButton { border: none; margin: 0; padding: 0; width: 54 }"
            "QToolButton:checked, QToolButton:pressed {"
            // Use border-image here, not background-image, because bfb_bg_active.png is 56px wide
            "     border-image: url(theme:/bfb_bg_active.png);"
            "}"
    );

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_button);

    m_dash_iface = new QDBusInterface(DBUS_SERVICE, DBUS_PATH, DBUS_IFACE);
    connect(m_dash_iface, SIGNAL(activeChanged(bool)), SLOT(dashActiveChanged(bool)));
}

HomeButtonApplet::~HomeButtonApplet()
{
    disconnect(this, SLOT(dashActiveChanged(bool)));
    delete m_dash_iface;
}

void HomeButtonApplet::dashActiveChanged(bool active)
{
    m_button->setChecked(active);
}

void HomeButtonApplet::slotButtonClicked()
{
    bool dashActive = m_dash_iface->property("active").toBool();

    if (dashActive) {
        m_dash_iface->setProperty("active", false);
    } else {
        /* Call local.DashDeclarativeView.activateHome (will set local.DashDeclarativeView.active to true */
        m_dash_iface->call(QDBus::Block, "activateHome");
    }
}

#include "homebuttonapplet.moc"

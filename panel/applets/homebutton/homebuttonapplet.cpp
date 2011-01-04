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
#include <QDBusServiceWatcher>

static const char* DBUS_SERVICE = "com.canonical.UnityQt";
static const char* DBUS_PATH = "/dash";
static const char* DBUS_IFACE = "local.DashDeclarativeView";

HomeButtonApplet::HomeButtonApplet()
: m_button(new QToolButton), m_dashInterface(NULL), m_serviceWatcher(NULL)
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

    m_dashInterface = new QDBusInterface(DBUS_SERVICE, DBUS_PATH, DBUS_IFACE);
    if (!m_dashInterface->isValid()) {
        /* If we fail to connect to the dash via DBus it may be because it was not started yet.
           In this case we monitor the session bus waiting for the dash to register, and when it does
           we connect to its activeChanged signal so we can update the button. */
        delete m_dashInterface;
        m_dashInterface = NULL;
        QDBusServiceWatcher* m_serviceWatcher = new QDBusServiceWatcher(DBUS_SERVICE, QDBusConnection::sessionBus(),
                                                                        QDBusServiceWatcher::WatchForRegistration);
        connect(m_serviceWatcher, SIGNAL(serviceRegistered(QString)), SLOT(serviceRegistered(QString)));
    }
    else {
        connect(m_dashInterface, SIGNAL(activeChanged(bool)), SLOT(dashActiveChanged(bool)));
    }
}

HomeButtonApplet::~HomeButtonApplet()
{
    disconnect(this, SLOT(dashActiveChanged(bool)));
    disconnect(this, SLOT(serviceRegistered(QString)));
    delete m_dashInterface;
    delete m_serviceWatcher;
}

void HomeButtonApplet::dashActiveChanged(bool active)
{
    m_button->setChecked(active);
}

void HomeButtonApplet::serviceRegistered(QString name)
{
    Q_UNUSED(name)

    m_dashInterface = new QDBusInterface(DBUS_SERVICE, DBUS_PATH, DBUS_IFACE);
    connect(m_dashInterface, SIGNAL(activeChanged(bool)), SLOT(dashActiveChanged(bool)));
}

void HomeButtonApplet::slotButtonClicked()
{
    if (m_dashInterface == NULL) return; // The dash is not active yet.

    bool dashActive = m_dashInterface->property("active").toBool();

    if (dashActive) {
        m_dashInterface->setProperty("active", false);
    } else {
        /* Call local.DashDeclarativeView.activateHome (will set local.DashDeclarativeView.active to true */
        m_dashInterface->call(QDBus::Block, "activateHome");
    }
}

#include "homebuttonapplet.moc"

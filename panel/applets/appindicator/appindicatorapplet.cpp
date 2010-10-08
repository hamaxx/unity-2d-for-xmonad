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
#include "appindicatorapplet.h"

// Local
#include "debug_p.h"
#include "sniitem.h"

// Qt
#include <QDBusConnection>

// System
#include <unistd.h>

static const char* WATCHER_SERVICE = "org.kde.StatusNotifierWatcher";
static const char* WATCHER_PATH = "/StatusNotifierWatcher";
static const char* WATCHER_IFACE = "org.kde.StatusNotifierWatcher";

AppIndicatorApplet::AppIndicatorApplet()
{
    setupDBus();
    setupUi();
    createItems();
}

void AppIndicatorApplet::setupDBus()
{
    QDBusConnection bus = QDBusConnection::sessionBus();

    m_watcher = new QDBusInterface(WATCHER_SERVICE, WATCHER_PATH, WATCHER_IFACE, bus, this);

    pid_t pid = getpid();
    QString name = QString("org.kde.StatusNotifierHost-%1").arg(pid);
    bus.registerService(name);
}

void AppIndicatorApplet::setupUi()
{
    m_layout = new QHBoxLayout(this);
    m_layout->setMargin(0);
    m_layout->setSpacing(0);
}

void AppIndicatorApplet::createItems()
{
    QStringList ids = m_watcher->property("RegisteredStatusNotifierItems").toStringList();
    Q_FOREACH(QString id, ids) {
        createItem(id);
    }
}

void AppIndicatorApplet::createItem(const QString& id)
{
    int slash = id.indexOf('/');
    UQ_RETURN_IF_FAIL(slash != -1);
    QString service = id.left(slash);
    QString path = id.mid(slash);

    UQ_VAR(service);
    UQ_VAR(path);

    SNIItem* item = new SNIItem(service, path, this);
    m_layout->addWidget(item);
}

#include "appindicatorapplet.moc"

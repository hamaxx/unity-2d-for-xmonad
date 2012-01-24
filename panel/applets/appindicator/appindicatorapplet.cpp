/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Aurélien Gâteau <aurelien.gateau@canonical.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

// Self
#include "appindicatorapplet.h"

// Local
#include "debug_p.h"
#include "sniitem.h"

// Qt
#include <QDBusConnection>
#include <QHBoxLayout>

// System
#include <unistd.h>

static const char* WATCHER_SERVICE = "org.kde.StatusNotifierWatcher";
static const char* WATCHER_PATH = "/StatusNotifierWatcher";
static const char* WATCHER_IFACE = "org.kde.StatusNotifierWatcher";

AppIndicatorApplet::AppIndicatorApplet(Unity2dPanel* panel)
: Unity2d::PanelApplet(panel)
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
    m_menuBar = new QMenuBar;
    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_menuBar);
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

    new SNIItem(service, path, m_menuBar);
}

#include "appindicatorapplet.moc"

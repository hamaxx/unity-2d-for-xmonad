/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Aurélien Gâteau <aurelien.gateau@canonical.com>
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "bfb.h"

// libunity-2d
#include <unity2dtr.h>
#include <debug_p.h>

// Qt
#include <QDBusInterface>
#include <QDBusPendingCall>

static const char* DASH_DBUS_SERVICE = "com.canonical.Unity2d.Dash";
static const char* DASH_DBUS_PATH = "/Dash";
static const char* DASH_DBUS_INTERFACE = "com.canonical.Unity2d.Dash";

BfbItem::BfbItem()
: m_dashInterface(new QDBusInterface(DASH_DBUS_SERVICE, DASH_DBUS_PATH, DASH_DBUS_INTERFACE))
{
    m_dashInterface->setParent(this);
}

BfbItem::~BfbItem()
{
}

bool BfbItem::active() const
{
    return m_dashInterface->property("active").toBool();
}

bool BfbItem::running() const
{
    return false;
}

int BfbItem::windowCount() const
{
    return 0;
}

bool BfbItem::urgent() const
{
    return false;
}

QString BfbItem::name() const
{
    return u2dTr("Dash");
}

QString BfbItem::icon() const
{
    return "unity-icon-theme/distributor-logo";
}

bool BfbItem::launching() const
{
    return false;
}

void BfbItem::activate()
{
    if (active()) {
        m_dashInterface->setProperty("active", false);
    } else {
        m_dashInterface->call(QDBus::Block, "activateHome");
    }
    activeChanged(active());
}

void BfbItem::createMenuActions()
{
}

////////////////////////////////////////////////////////////
BfbModel::BfbModel(QObject* parent)
: QAbstractListModel(parent)
, m_bfbItem(new BfbItem)
{
}

BfbModel::~BfbModel()
{
    delete m_bfbItem;
}

int BfbModel::rowCount(const QModelIndex& /*parent*/) const
{
    return 1;
}

QVariant BfbModel::data(const QModelIndex& index, int /*role*/) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    return QVariant::fromValue(m_bfbItem);
}

#include <bfb.moc>

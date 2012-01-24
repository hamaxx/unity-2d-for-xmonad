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
#include <dashclient.h>
#include <debug_p.h>
#include <unity2dtr.h>

// Qt

BfbItem::BfbItem()
: m_active(false)
{
    connect(DashClient::instance(), SIGNAL(activePageChanged(const QString&)),
        SLOT(slotActivePageChanged(const QString&)));
}

BfbItem::~BfbItem()
{
}

bool BfbItem::active() const
{
    return m_active;
}

void BfbItem::slotActivePageChanged(const QString& page)
{
    bool active = !page.isEmpty();
    if (m_active != active) {
        m_active = active;
        activeChanged(m_active);
    }
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
    return u2dTr("Dash home");
}

QString BfbItem::icon() const
{
    return "/usr/share/unity/4/launcher_bfb.png";
}

bool BfbItem::launching() const
{
    return false;
}

void BfbItem::activate()
{
    DashClient::instance()->setActivePage(m_active ? "" : "home");
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

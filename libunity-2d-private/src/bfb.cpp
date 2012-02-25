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

#include "config.h"
#include "bfb.h"

// libunity-2d
#include <debug_p.h>
#include <unity2dtr.h>

// Qt

BfbItem::BfbItem()
: m_active(false), m_view(NULL)
{
}

BfbItem::~BfbItem()
{
}

bool BfbItem::active() const
{
    return m_active;
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
    return QString(UNITY_DIR) + "5/launcher_bfb.png";
}

bool BfbItem::launching() const
{
    return false;
}

QObject* BfbItem::dashView() const
{
    return m_view;
}

void BfbItem::setDashView(QObject* view)
{
    if (m_view != NULL) {
        disconnect(view);
    }
    m_view = view;
    if (m_view != NULL) {
        connect(view, SIGNAL(dashActiveChanged(bool)), this, SLOT(slotDashActiveChanged(bool)));
    }
}

void BfbItem::activate()
{
    Q_ASSERT(m_view != NULL);
    if (m_view != NULL) {
        QMetaObject::invokeMethod(m_view, "toggleDash");
    }
}

void BfbItem::createMenuActions()
{
}

void BfbItem::slotDashActiveChanged(bool active)
{
    if (m_active != active) {
        m_active = active;
        Q_EMIT activeChanged(m_active);
    }
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

QObject* BfbModel::dashView() const
{
    return m_bfbItem->dashView();
}

void BfbModel::setDashView(QObject* view)
{
    m_bfbItem->setDashView(view);
}

#include <bfb.moc>

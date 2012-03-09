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
: m_active(false), m_activeScreen(-1), m_manager(NULL)
{
}

BfbItem::~BfbItem()
{
}

bool BfbItem::active() const
{
    return m_active;
}

int  BfbItem::activeScreen() const
{
    if (active()) {
        return m_activeScreen;
    } else {
        return -1;
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
    return QString(UNITY_DIR) + "5/launcher_bfb.png";
}

bool BfbItem::launching() const
{
    return false;
}

QObject* BfbItem::dashManager() const
{
    return m_manager;
}

void BfbItem::setDashManager(QObject* manager)
{
    if (m_manager != NULL) {
        disconnect(m_manager);
    }
    m_manager = manager;
    if (m_manager != NULL) {
        connect(manager, SIGNAL(dashActiveChanged(bool)), this, SLOT(slotDashActiveChanged(bool)));
        connect(manager, SIGNAL(dashScreenChanged(int)), this, SLOT(slotDashScreenChanged(int)));
    }
}

void BfbItem::activate()
{
    Q_ASSERT(m_manager != NULL);
    if (m_manager != NULL) {
        QMetaObject::invokeMethod(m_manager, "toggleDash");
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
        if (m_active) {
            Q_EMIT activeScreenChanged(m_activeScreen);
        }
    }
}

void BfbItem::slotDashScreenChanged(int activeScreen)
{
    if (m_activeScreen != activeScreen) {
        m_activeScreen = activeScreen;
        if (m_active) {
            Q_EMIT activeScreenChanged(m_activeScreen);
        }
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

QObject* BfbModel::dashManager() const
{
    return m_bfbItem->dashManager();
}

void BfbModel::setDashManager(QObject* manager)
{
    m_bfbItem->setDashManager(manager);
}

#include <bfb.moc>

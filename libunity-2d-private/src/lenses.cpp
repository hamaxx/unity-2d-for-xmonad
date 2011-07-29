/*
 * Copyright (C) 2011 Canonical, Ltd.
 *
 * Authors:
 *  Florian Boucault <florian.boucault@canonical.com>
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

// Self
#include "lenses.h"

// Local
#include "lens.h"

// Qt

// libunity-core
#include <UnityCore/FilesystemLenses.h>

Lenses::Lenses(QObject *parent) :
    QAbstractListModel(parent)
{
    m_unityLenses = new unity::dash::FilesystemLenses("/home/ubuntu/dev/unity/tests/data/lenses");
    // FIXME: should connect to 'count' changed signal
}

Lenses::~Lenses()
{
    delete m_unityLenses;
}

int Lenses::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return m_unityLenses->count();
}

QVariant Lenses::data(const QModelIndex& index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }

    unity::dash::Lens::Ptr unityLens = m_unityLenses->GetLensAtIndex(index.row());
    Lens* lens = new Lens();
    lens->setUnityLens(unityLens);

    return QVariant::fromValue(lens);
}

#include "lenses.moc"

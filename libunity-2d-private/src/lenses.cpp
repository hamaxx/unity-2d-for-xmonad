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
    QHash<int, QByteArray> roles;
    roles[Lenses::RoleItem] = "item";
    roles[Lenses::RoleVisible] = "visible";
    setRoleNames(roles);

    m_unityLenses = new unity::dash::FilesystemLenses("/usr/share/unity/lenses");
    for (unsigned int i=0; i<m_unityLenses->count(); i++) {
        unity::dash::Lens::Ptr unityLens = m_unityLenses->GetLensAtIndex(i);
        addUnityLens(unityLens, i);
    }
    m_unityLenses->lens_added.connect(sigc::mem_fun(this, &Lenses::onLensAdded));
}

Lenses::~Lenses()
{
    delete m_unityLenses;
}

int Lenses::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)

    return m_lenses.count();
}

QVariant Lenses::data(const QModelIndex& index, int role) const
{
    Q_UNUSED(role)

    if (!index.isValid()) {
        return QVariant();
    }

    Lens* lens = m_lenses.at(index.row());

    if (role == Lenses::RoleItem) {
        return QVariant::fromValue(lens);
    } else if (role == Lenses::RoleVisible) {
        return QVariant::fromValue(lens->visible());
    } else {
        return QVariant();
    }
}

QVariant Lenses::get(int row) const
{
    return data(QAbstractListModel::index(row), 0);
}

QVariant Lenses::get(const QString& lens_id) const
{
    Q_FOREACH(Lens* lens, m_lenses) {
        if (lens->id() == lens_id) {
            return QVariant::fromValue(lens);
        }
    }

    return QVariant();
}

void Lenses::onLensAdded(unity::dash::Lens::Ptr& lens)
{
    int index = m_lenses.count();
    beginInsertRows(QModelIndex(), index, index);
    addUnityLens(lens, index);
    endInsertRows();
}

void Lenses::onLensPropertyChanged()
{
    QModelIndex lensIndex = index(m_lenses.indexOf(qobject_cast<Lens*>(sender())));
    Q_EMIT dataChanged(lensIndex, lensIndex);
}

void Lenses::addUnityLens(unity::dash::Lens::Ptr unity_lens, int index)
{
    Lens* lens = new Lens();
    lens->setUnityLens(unity_lens);
    /* DOCME */
    QObject::connect(lens, SIGNAL(visibleChanged(bool)), this, SLOT(onLensPropertyChanged()));
    m_lenses.insert(index, lens);
}

void Lenses::removeUnityLens(int index)
{
    Lens* lens = m_lenses.takeAt(index);
    delete lens;
}

#include "lenses.moc"

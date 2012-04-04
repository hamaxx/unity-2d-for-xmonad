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
#include "debug_p.h"
#include "lens.h"
#include "hotkey.h"
#include "hotkeymonitor.h"

// Qt
#include <QKeySequence>

// libunity-core
#include <UnityCore/FilesystemLenses.h>
#include <UnityCore/HomeLens.h>
#include <unity2dtr.h>

Lenses::Lenses(QObject *parent) :
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[Lenses::RoleItem] = "item";
    roles[Lenses::RoleVisible] = "visible";
    setRoleNames(roles);

    m_homeLens = new unity::dash::HomeLens(u2dTr("Home").toStdString(), u2dTr("Home screen").toStdString(), u2dTr("Search").toStdString());
    m_unityLenses = new unity::dash::FilesystemLenses();
    m_homeLens->AddLenses(*m_unityLenses);
    m_homeLens->lens_added.connect(sigc::mem_fun(this, &Lenses::onLensAdded));
    unity::dash::HomeLens::Ptr homeLensPtr(m_homeLens);
    addUnityLens(homeLensPtr, 0);

    connect(&m_shortcutMapper, SIGNAL(mapped(QString)), this, SIGNAL(activateLensRequested(QString)));
}

Lenses::~Lenses()
{
    delete m_unityLenses;
    delete m_homeLens;
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

    setLensShortcut(lens);
    connect(lens, SIGNAL(shortcutChanged(std::string)), this, SLOT(onLensShortcutChanged()));
}

void Lenses::removeUnityLens(int index)
{
    Lens* lens = m_lenses.takeAt(index);

    Hotkey *hk = m_lensShorcuts.take(lens);
    if (hk != NULL) {
        m_shortcutMapper.removeMappings(hk);
    }

    delete lens;
}

void Lenses::onLensShortcutChanged()
{
    Lens *lens = qobject_cast<Lens*>(sender());
    Q_ASSERT(lens != 0);
    if (lens != 0) {
        setLensShortcut(lens);
    }
}

void Lenses::setLensShortcut(Lens *lens)
{
    Hotkey *hk = m_lensShorcuts.take(lens);
    if (hk != NULL) {
        m_shortcutMapper.removeMappings(hk);
    }
    if (!lens->shortcut().isEmpty()) {
        const QKeySequence ks(lens->shortcut());
        if (ks.count() == 1) {
            hk = HotkeyMonitor::instance().getHotkeyFor((Qt::Key)ks[0], Qt::MetaModifier);
            m_shortcutMapper.setMapping(hk, lens->id());
            connect(hk, SIGNAL(pressed()), &m_shortcutMapper, SLOT(map()));
        } else {
            UQ_WARNING << "Couldn't parse shortcut for Lens. Shorcut is " << lens->shortcut();
        }
    }
}

#include "lenses.moc"

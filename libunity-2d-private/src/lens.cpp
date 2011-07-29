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
#include "lens.h"

Lens::Lens(QObject *parent) :
    QObject(parent)
{   
}

QString Lens::id() const
{
    return QString::fromStdString(m_unityLens->id());
}

QString Lens::dbusName() const
{
    return QString::fromStdString(m_unityLens->dbus_name());
}

QString Lens::dbusPath() const
{
    return QString::fromStdString(m_unityLens->dbus_path());
}

QString Lens::name() const
{
    return QString::fromStdString(m_unityLens->name());
}

QString Lens::icon() const
{
    return QString::fromStdString(m_unityLens->icon());
}

QString Lens::description() const
{
    return QString::fromStdString(m_unityLens->description());
}

QString Lens::searchHint() const
{
    return QString::fromStdString(m_unityLens->search_hint());
}

bool Lens::visible() const
{
    return m_unityLens->visible();
}

QString Lens::shortcut() const
{
    return QString::fromStdString(m_unityLens->shortcut());
}

void Lens::setUnityLens(unity::dash::Lens::Ptr lens)
{
    if (m_unityLens != NULL) {
        // FIXME: should disconnect from m_unityLens's properties changed signal
    }

    m_unityLens = lens;
    m_unityLens->id.changed.connect(sigc::mem_fun(this, &Lens::idChanged));
    m_unityLens->dbus_name.changed.connect(sigc::mem_fun(this, &Lens::dbusNameChanged));
    m_unityLens->dbus_path.changed.connect(sigc::mem_fun(this, &Lens::dbusPathChanged));
    m_unityLens->name.changed.connect(sigc::mem_fun(this, &Lens::nameChanged));
    m_unityLens->icon.changed.connect(sigc::mem_fun(this, &Lens::iconChanged));
    m_unityLens->description.changed.connect(sigc::mem_fun(this, &Lens::descriptionChanged));
    m_unityLens->search_hint.changed.connect(sigc::mem_fun(this, &Lens::searchHintChanged));
    m_unityLens->visible.changed.connect(sigc::mem_fun(this, &Lens::visibleChanged));
    m_unityLens->shortcut.changed.connect(sigc::mem_fun(this, &Lens::shortcutChanged));
}

#include "lens.moc"

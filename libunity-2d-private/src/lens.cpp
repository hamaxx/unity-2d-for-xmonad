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
    m_results = new DeeListModel;
    m_globalResults = new DeeListModel;
    m_categories = new DeeListModel;
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

QString Lens::iconHint() const
{
    return QString::fromStdString(m_unityLens->icon_hint());
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

bool Lens::searchInGlobal() const
{
    return m_unityLens->search_in_global();
}

QString Lens::shortcut() const
{
    return QString::fromStdString(m_unityLens->shortcut());
}

bool Lens::connected() const
{
    return m_unityLens->connected();
}

DeeListModel* Lens::results() const
{
    return m_results;
}

DeeListModel* Lens::globalResults() const
{
    return m_globalResults;
}

DeeListModel* Lens::categories() const
{
    return m_categories;
}

bool Lens::active() const
{
    return m_unityLens->active();
}

void Lens::globalSearch(const QString& search_string)
{
    m_unityLens->GlobalSearch(search_string.toStdString());
}

void Lens::search(const QString& search_string)
{
    m_unityLens->Search(search_string.toStdString());
}

void Lens::setUnityLens(unity::dash::Lens::Ptr lens)
{
    if (m_unityLens != NULL) {
        // FIXME: should disconnect from m_unityLens's signals
    }

    // FIXME: should emit change notification signals for all properties

    m_unityLens = lens;

    m_results->setName(QString::fromStdString(m_unityLens->results()->swarm_name));

    /* Property change signals */
    m_unityLens->id.changed.connect(sigc::mem_fun(this, &Lens::idChanged));
    m_unityLens->dbus_name.changed.connect(sigc::mem_fun(this, &Lens::dbusNameChanged));
    m_unityLens->dbus_path.changed.connect(sigc::mem_fun(this, &Lens::dbusPathChanged));
    m_unityLens->name.changed.connect(sigc::mem_fun(this, &Lens::nameChanged));
    m_unityLens->icon_hint.changed.connect(sigc::mem_fun(this, &Lens::iconHintChanged));
    m_unityLens->description.changed.connect(sigc::mem_fun(this, &Lens::descriptionChanged));
    m_unityLens->search_hint.changed.connect(sigc::mem_fun(this, &Lens::searchHintChanged));
    m_unityLens->visible.changed.connect(sigc::mem_fun(this, &Lens::visibleChanged));
    m_unityLens->search_in_global.changed.connect(sigc::mem_fun(this, &Lens::searchInGlobalChanged));
    m_unityLens->shortcut.changed.connect(sigc::mem_fun(this, &Lens::shortcutChanged));
    m_unityLens->connected.changed.connect(sigc::mem_fun(this, &Lens::connectedChanged));
    m_unityLens->results.changed.connect(sigc::mem_fun(this, &Lens::onResultsChanged));
    m_unityLens->results()->swarm_name.changed.connect(sigc::mem_fun(this, &Lens::onResultsSwarmNameChanged));
    m_unityLens->global_results.changed.connect(sigc::mem_fun(this, &Lens::onGlobalResultsChanged));
    m_unityLens->global_results()->swarm_name.changed.connect(sigc::mem_fun(this, &Lens::onGlobalResultsSwarmNameChanged));
    m_unityLens->categories.changed.connect(sigc::mem_fun(this, &Lens::onCategoriesChanged));
    m_unityLens->categories()->swarm_name.changed.connect(sigc::mem_fun(this, &Lens::onCategoriesSwarmNameChanged));
    m_unityLens->active.changed.connect(sigc::mem_fun(this, &Lens::activeChanged));

    /* Signals forwarding */
    m_unityLens->search_finished.connect(sigc::mem_fun(this, &Lens::searchFinished));
    m_unityLens->global_search_finished.connect(sigc::mem_fun(this, &Lens::globalSearchFinished));
}

void Lens::onResultsSwarmNameChanged(std::string swarm_name)
{
    m_results->setName(QString::fromStdString(m_unityLens->results()->swarm_name));
}

void Lens::onResultsChanged(unity::dash::Results::Ptr results)
{
    m_results->setName(QString::fromStdString(m_unityLens->results()->swarm_name));
}

void Lens::onGlobalResultsSwarmNameChanged(std::string swarm_name)
{
    m_globalResults->setName(QString::fromStdString(m_unityLens->global_results()->swarm_name));
}

void Lens::onGlobalResultsChanged(unity::dash::Results::Ptr global_results)
{
    m_globalResults->setName(QString::fromStdString(m_unityLens->global_results()->swarm_name));
}

void Lens::onCategoriesSwarmNameChanged(std::string swarm_name)
{
    m_categories->setName(QString::fromStdString(m_unityLens->categories()->swarm_name));
}

void Lens::onCategoriesChanged(unity::dash::Categories::Ptr categories)
{
    m_categories->setName(QString::fromStdString(m_unityLens->categories()->swarm_name));
}

#include "lens.moc"

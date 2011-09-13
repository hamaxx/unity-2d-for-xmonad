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

// libunity-2d
#include <debug_p.h>
#include "launcherapplication.h"
#include "filters.h"

// Qt
#include <QUrl>
#include <QDesktopServices>

Lens::Lens(QObject *parent) :
    QObject(parent)
{
    m_results = new DeeListModel(this);
    m_globalResults = new DeeListModel(this);
    m_categories = new DeeListModel(this);
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

Filters* Lens::filters() const
{
    return m_filters;
}

QString Lens::searchQuery() const
{
    return m_searchQuery;
}

QString Lens::globalSearchQuery() const
{
    return m_globalSearchQuery;
}

void Lens::setActive(bool active)
{
    m_unityLens->active = active;
}

void Lens::setSearchQuery(const QString& search_query)
{
    /* Checking for m_searchQuery.isNull() which returns true only when the string
       has never been set is necessary because when search_query is the empty
       string ("") and m_searchQuery is the null string,
       search_query != m_searchQuery is still true.
    */
    if (m_searchQuery.isNull() || search_query != m_searchQuery) {
        m_searchQuery = search_query;
        m_unityLens->Search(search_query.toStdString());
        Q_EMIT searchQueryChanged();
    }
}

void Lens::setGlobalSearchQuery(const QString& search_query)
{
    /* Checking for m_globalSearchQuery.isNull() which returns true only when the string
       has never been set is necessary because when search_query is the empty
       string ("") and m_globalSearchQuery is the null string,
       search_query != m_globalSearchQuery is still true.
    */
    if (m_globalSearchQuery.isNull() || search_query != m_globalSearchQuery) {
        m_globalSearchQuery = search_query;
        m_unityLens->GlobalSearch(search_query.toStdString());
        Q_EMIT globalSearchQueryChanged();
    }
}

void Lens::activate(const QString& uri)
{
    m_unityLens->Activate(uri.toStdString());
}

void Lens::onActivated(std::string const& uri, unity::dash::HandledType type, unity::dash::Lens::Hints const&)
{
    if (type == unity::dash::NOT_HANDLED) {
        fallbackActivate(QString::fromStdString(uri));
    }
}

void Lens::fallbackActivate(const QString& uri)
{
    /* FIXME: stripping all content before the first column because for some
              reason the lenses give uri with junk content at their beginning.
    */
    QString tweakedUri = uri;
    int firstColumnAt = tweakedUri.indexOf(":");
    tweakedUri.remove(0, firstColumnAt+1);

    /* Tries various methods to trigger a sensible action for the given 'uri'.
       If it has no understanding of the given scheme it falls back on asking
       Qt to open the uri.
    */
    QUrl url(tweakedUri);
    if (url.scheme() == "file") {
        /* Override the files place's default URI handler: we want the file
           manager to handle opening folders, not the dash.

           Ref: https://bugs.launchpad.net/upicek/+bug/689667
        */
        QDesktopServices::openUrl(url);
        return;
    }
    if (url.scheme() == "application") {
        LauncherApplication application;
        /* Cannot set the desktop file to url.host(), because the QUrl constructor
           converts the host name to lower case to conform to the Nameprep
           RFC (see http://doc.qt.nokia.com/qurl.html#FormattingOption-enum).
           Ref: https://bugs.launchpad.net/unity-place-applications/+bug/784478 */
        QString desktopFile = tweakedUri.mid(tweakedUri.indexOf("://") + 3);
        application.setDesktopFile(desktopFile);
        application.activate();
        return;
    }

    UQ_WARNING << "FIXME: Possibly no handler for scheme: " << url.scheme();
    UQ_WARNING << "Trying to open" << tweakedUri;
    /* Try our luck */
    QDesktopServices::openUrl(url);
}

void Lens::setUnityLens(unity::dash::Lens::Ptr lens)
{
    if (m_unityLens != NULL) {
        // FIXME: should disconnect from m_unityLens's signals
        delete m_filters;
    }

    m_unityLens = lens;

    m_filters = new Filters(m_unityLens->filters, this);

    m_results->setName(QString::fromStdString(m_unityLens->results()->swarm_name));
    m_globalResults->setName(QString::fromStdString(m_unityLens->global_results()->swarm_name));
    m_categories->setName(QString::fromStdString(m_unityLens->categories()->swarm_name));

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

    /* FIXME: signal should be forwarded instead of calling the handler directly */
    m_unityLens->activated.connect(sigc::mem_fun(this, &Lens::onActivated));

    /* Synchronize local states with m_unityLens right now and whenever
       m_unityLens becomes connected */
    /* FIXME: should emit change notification signals for all properties */
    connect(this, SIGNAL(connectedChanged(bool)), SLOT(synchronizeStates()));
    synchronizeStates();
}

void Lens::synchronizeStates()
{
    if (connected()) {
        /* Forward local states to m_unityLens */
        if (!m_searchQuery.isNull()) {
            m_unityLens->Search(m_searchQuery.toStdString());
        }
        if (!m_globalSearchQuery.isNull()) {
            m_unityLens->GlobalSearch(m_globalSearchQuery.toStdString());
        }
    }
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

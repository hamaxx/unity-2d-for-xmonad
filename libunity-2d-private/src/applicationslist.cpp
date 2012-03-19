/*
 * Copyright (C) 2010-2011 Canonical, Ltd.
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

#include "application.h"
#include "applicationslist.h"
#include "applicationslistmanager.h"
#include "webfavorite.h"

#include "bamf-matcher.h"
#include "bamf-application.h"
#include "gconfitem-qml-wrapper.h"

// unity-2d
#include "config.h"
#include <debug_p.h>

#include <QStringList>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QX11Info>

#include <debug_p.h>

extern "C" {
#include <libsn/sn.h>
}

/* List of executables that are too generic to be matched against a single application. */
static const QStringList EXECUTABLES_BLACKLIST = (QStringList() << "xdg-open");
static const QByteArray LATEST_SETTINGS_MIGRATION = "3.2.10";

ApplicationsList::ApplicationsList(QObject *parent) :
    QAbstractListModel(parent)
{
    /* Register the display to receive startup notifications */
    Display *xdisplay = QX11Info::display();
    m_snDisplay = sn_display_new(xdisplay, NULL, NULL);
    m_snContext = sn_monitor_context_new(m_snDisplay, QX11Info::appScreen(),
                                          ApplicationsList::snEventHandler,
                                          this, NULL);
    Unity2dApplication* application = Unity2dApplication::instance();
    if (application == NULL) {
        /* This can happen for example when using qmlviewer to run the launcher */
        UQ_WARNING << "The application is not an Unity2dApplication."
                      "Applications startup notifications will be ignored.";
    } else {
        application->installX11EventFilter(this);
    }

    /* Get the system applications data dirs and be flexible if / is not at the
       end of each path. */
    QString xdgDataDir = QFile::decodeName(getenv("XDG_DATA_DIRS"));
    if (xdgDataDir.isEmpty()) {
        xdgDataDir = "/usr/local/share/:/usr/share/";
    }
    Q_FOREACH(const QString& dirName, xdgDataDir.split(':')) {
      m_xdgApplicationDirs << QDir::cleanPath(dirName + "/applications") + "/";
    }

    load();

    ApplicationsListManager::instance()->addList(this);
}

void
ApplicationsList::snEventHandler(SnMonitorEvent *event, void *user_data)
{
    /* This method is static and only forwards the event to a non static method. */
    ((ApplicationsList*)user_data)->onSnMonitorEventReceived(event);
}

void
ApplicationsList::onSnMonitorEventReceived(SnMonitorEvent *event)
{
    SnStartupSequence *sequence = sn_monitor_event_get_startup_sequence(event);

    switch (sn_monitor_event_get_type (event)) {
        case SN_MONITOR_EVENT_INITIATED:
            insertSnStartupSequence(sequence);
            break;
        case SN_MONITOR_EVENT_CHANGED:
        case SN_MONITOR_EVENT_COMPLETED:
        case SN_MONITOR_EVENT_CANCELED:
            /* These events are ignored for now. This is acceptable since the
               case of a failed application startup is handled by
               Application::launching being automatically reset to
               false after a timeout. */
            break;
    }
}


bool
ApplicationsList::x11EventFilter(XEvent* xevent)
{
    /* libsn specifies that all events need to be forwarded to
       sn_display_process_event but it is not actually necessary.
       Forwarding only the events of type ClientMessage.
     */
    if (xevent->type == ClientMessage) {
        sn_display_process_event(m_snDisplay, xevent);
    }
    return false;
}

void ApplicationsList::remoteEntryUpdated(const QString& desktopFile, const QString& sender, const QString& applicationURI, const QMap<QString, QVariant>& properties)
{
    Q_FOREACH(Application *application, m_applications) {
        if (QFileInfo(application->desktop_file()).fileName() == desktopFile) {
            application->updateOverlaysState(sender, properties);
            return;
        }
    }

    UQ_WARNING << "Application sent an update but we don't seem to have it in the launcher:" << applicationURI;
}

ApplicationsList::~ApplicationsList()
{
    ApplicationsListManager::instance()->removeList(this);

    sn_monitor_context_unref(m_snContext);
    sn_display_unref(m_snDisplay);

    qDeleteAll(m_applications);
}

QString
ApplicationsList::favoriteFromDesktopFilePath(const QString& _desktopFile) const
{
    QString desktopFile(_desktopFile);
    Q_FOREACH(const QString& applicationDir, m_xdgApplicationDirs) {
        if (_desktopFile.startsWith(applicationDir)) {
            desktopFile.remove(applicationDir);
            desktopFile.replace("/", "-");
            break;
        }
    }
    return desktopFile;
}

void
ApplicationsList::insertApplication(Application* application)
{
    /* Insert at the end of the list. */
    int index = m_applications.size();

    beginInsertRows(QModelIndex(), index, index);
    m_applications.insert(index, application);

    if (!application->desktop_file().isEmpty()) {
        m_applicationForDesktopFile.insert(application->desktop_file(), application);
    }
    QString executable = application->executable();
    if (!executable.isEmpty() && !EXECUTABLES_BLACKLIST.contains(executable)) {
        m_applicationForExecutable.insert(executable, application);
    }
    endInsertRows();

    QObject::connect(application, SIGNAL(closed()), this, SLOT(onApplicationClosed()));
    QObject::connect(application, SIGNAL(stickyChanged(bool)), this, SLOT(onApplicationStickyChanged(bool)));
    QObject::connect(application, SIGNAL(launchingChanged(bool)), this, SLOT(onApplicationLaunchingChanged(bool)));
    QObject::connect(application, SIGNAL(urgentChanged(bool)), this, SLOT(onApplicationUrgentChanged(bool)));
}

void
ApplicationsList::removeApplication(Application* application)
{
    int index = m_applications.indexOf(application);

    if (index == -1) {
        /* application is not present in m_applications */
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_applications.removeAt(index);
    m_applicationForDesktopFile.remove(application->desktop_file());
    m_applicationForExecutable.remove(application->executable());
    endRemoveRows();

    /* ApplicationsList::removeApplication might have been called in
       response to a signal emitted by application itself. Do not delete
       immediately to cater for this case.
    */
    application->deleteLater();
}

void
ApplicationsList::onApplicationUserVisibleChanged(bool user_visible)
{
    BamfApplication* bamf_application = qobject_cast<BamfApplication*>(sender());
    if (user_visible) {
        insertBamfApplication(bamf_application);
    } else {
        /* FIXME: this case has not been implemented yet but it has not been
           affecting anybody so far. */
    }
}

void ApplicationsList::insertBamfApplication(BamfApplication* bamf_application)
{
    /* Only insert BamfApplications for which the user_visible property is true.
       Monitor that property so that they are inserted/removed dynamically when it changes.

       Not doing it led to KDE3 applications not showing up in the launcher.
       Ref.: https://bugs.launchpad.net/unity-2d/+bug/719983
    */
    QObject::connect(bamf_application, SIGNAL(UserVisibleChanged(bool)), this, SLOT(onApplicationUserVisibleChanged(bool)), Qt::UniqueConnection);

    if (!bamf_application->user_visible()) {
        return;
    }

    Application* matchingApplication = NULL;
    Application* newApplication = new Application;
    newApplication->setBamfApplication(bamf_application);

    QString executable = newApplication->executable();
    QString desktop_file = newApplication->desktop_file();
    if (m_applicationForDesktopFile.contains(desktop_file)) {
        /* A Application with the same desktop file already exists */
        matchingApplication = m_applicationForDesktopFile[desktop_file];
    } else if (m_applicationForExecutable.contains(executable)) {
        /* A Application with the same executable already exists */
        matchingApplication = m_applicationForExecutable[executable];
        /* If the application already registered for that executable has a
           desktop file assigned then make sure that the one to be inserted
           has the same desktop file.
        */
        QString matchingDesktopFile = matchingApplication->desktop_file();
        if (!matchingDesktopFile.isEmpty() && !desktop_file.isEmpty() &&
            matchingDesktopFile != desktop_file) {
                matchingApplication = NULL;
        }
    }

    if (matchingApplication != NULL) {
        /* A Application that corresponds to bamf_application already exists */
        /* FIXME: this deletion blocks for a long time (around 100ms here) and
           leads to a visual glitch in the launcher when an application finished
           starting up. This is due to the deletion of the QFileSystemWatcher
           belonging to the Application. */
        delete newApplication;
        matchingApplication->setBamfApplication(bamf_application);
    } else {
        insertApplication(newApplication);
    }
}

void
ApplicationsList::insertFavoriteApplication(const QString& desktop_file)
{
    if (m_applicationForDesktopFile.contains(desktop_file)) {
        return;
    }

    /* Create a new Application */
    Application* application = new Application;
    application->setDesktopFile(desktop_file);

    /* If the desktop_file property is empty after setting it, it
       means glib couldn't load the desktop file (probably corrupted) */
    if (application->desktop_file().isEmpty()) {
        UQ_WARNING << "Favorite application not added due to desktop file missing or corrupted ("
                   << desktop_file << ")";
        delete application;
    } else {
        /* Register favorite desktop file into BAMF: applications with the same
           executable file will match with the given desktop file. This replicates
           the behaviour of Unity that does it automatically when calling libbamf's
           bamf_matcher_get_application_for_desktop_file.
           It fixes bug https://bugs.launchpad.net/unity-2d/+bug/739454
           The need for that API call is odd and causes at least one bug:
           https://bugs.launchpad.net/unity/+bug/762898
        */
        BamfMatcher& matcher = BamfMatcher::get_default();
        matcher.register_favorites(QStringList(application->desktop_file()));

        insertApplication(application);
        application->setSticky(true);
    }
}

void
ApplicationsList::insertWebFavorite(const QUrl& url)
{
    if (!url.isValid() || url.isRelative()) {
        UQ_WARNING << "Invalid URL:" << url;
        return;
    }

    Application* application = new Application;
    WebFavorite* webfav = new WebFavorite(url, application);

    application->setDesktopFile(webfav->desktopFile());
    insertApplication(application);
    application->setSticky(true);
}

void
ApplicationsList::insertSnStartupSequence(SnStartupSequence* sequence)
{
    if (sequence == NULL) {
        return;
    }

    QString executable = sn_startup_sequence_get_binary_name(sequence);
    if (EXECUTABLES_BLACKLIST.contains(executable)) {
        return;
    }

    if (m_applicationForExecutable.contains(executable)) {
        /* A Application with the same executable already exists */
        m_applicationForExecutable[executable]->setSnStartupSequence(sequence);
    } else {
        /* Create a new Application and append it to the list */
        Application* newApplication = new Application;
        newApplication->setSnStartupSequence(sequence);
        insertApplication(newApplication);
    }
}

void
ApplicationsList::load()
{
    /* Migrate the favorites if needed and ignore errors */
    QByteArray latest_migration = launcherConfiguration().property("favoriteMigration").toString().toAscii();
    if (latest_migration < LATEST_SETTINGS_MIGRATION) {
        if(QProcess::execute(INSTALL_PREFIX "/lib/unity/migrate_favorites.py") != 0) {
            UQ_WARNING << "Unable to run the migrate favorites tool successfully";
        }
    }

    /* Insert favorites */
    QString desktop_file;
    QStringList favorites = launcherConfiguration().property("favorites").toStringList();

    Q_FOREACH(const QString& favorite, favorites) {
       insertFavoriteApplication(favorite);
    }

    /* Insert running applications from Bamf */
    BamfMatcher& matcher = BamfMatcher::get_default();
    QScopedPointer<BamfApplicationList> running_applications(matcher.running_applications());
    BamfApplication* bamf_application;

    for(int i=0; i<running_applications->size(); i++) {
        bamf_application = running_applications->at(i);
        insertBamfApplication(bamf_application);
    }

    QObject::connect(&matcher, SIGNAL(ViewOpened(BamfView*)), SLOT(onBamfViewOpened(BamfView*)));
}

void
ApplicationsList::onBamfViewOpened(BamfView* bamf_view)
{
    /* Make sure bamf_view is in fact a BamfApplication */
    BamfApplication* bamf_application;
    bamf_application = dynamic_cast<BamfApplication*>(bamf_view);

    if (bamf_application == NULL) {
        return;
    }

    insertBamfApplication(bamf_application);
}

void ApplicationsList::onApplicationClosed()
{
    Application* application = static_cast<Application*>(sender());

    if (!application->sticky() && !application->running()) {
        removeApplication(application);
    }
}

void
ApplicationsList::onApplicationStickyChanged(bool sticky)
{
    Application* application = static_cast<Application*>(sender());

    writeFavoritesToGConf();

    if (!sticky && !application->running()) {
        removeApplication(application);
    }
}

void
ApplicationsList::onApplicationLaunchingChanged(bool launching)
{
    Application* application = static_cast<Application*>(sender());

    if (!application->sticky() && !application->running() && !application->launching()) {
        removeApplication(application);
    }
}

void
ApplicationsList::onApplicationUrgentChanged(bool urgent)
{
    Application* application = static_cast<Application*>(sender());
    if (urgent) {
        Q_EMIT applicationBecameUrgent(m_applications.indexOf(application));
    }
}

void
ApplicationsList::writeFavoritesToGConf()
{
    QStringList favorites;

    Q_FOREACH(Application *application, m_applications) {
        QString desktop_file = application->desktop_file();
        if (application->sticky()) {
            favorites.append(favoriteFromDesktopFilePath(desktop_file));
        }
    }

    launcherConfiguration().blockSignals(true);
    launcherConfiguration().setProperty("favorites", QVariant(favorites));
    launcherConfiguration().blockSignals(false);
}

int
ApplicationsList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_applications.size();
}

QVariant
ApplicationsList::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);

    if (!index.isValid()) {
        return QVariant();
    }

    return QVariant::fromValue(m_applications.at(index.row()));
}

void
ApplicationsList::moveFinished(int from, int to)
{
    Q_FOREACH(ApplicationsList *other, ApplicationsListManager::instance()->m_lists) {
        if (other != this) {
            other->doMove(from, to);
        }
    }
}

void
ApplicationsList::doMove(int from, int to)
{
    QModelIndex parent;
    /* When moving an item down, the destination index needs to be incremented
       by one, as explained in the documentation:
       http://doc.qt.nokia.com/qabstractitemmodel.html#beginMoveRows */
    beginMoveRows(parent, from, from, parent, to + (to > from ? 1 : 0));
    m_applications.move(from, to);
    endMoveRows();
}

void
ApplicationsList::move(int from, int to)
{
    doMove(from, to);

    if (m_applications[from]->sticky() || m_applications[to]->sticky()) {
        /* Update favorites only if at least one of the applications is a favorite */
        writeFavoritesToGConf();
    }
}

#include "applicationslist.moc"

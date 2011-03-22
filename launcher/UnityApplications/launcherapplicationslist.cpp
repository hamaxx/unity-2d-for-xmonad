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

#include "launcherapplication.h"
#include "launcherapplicationslist.h"
#include "webfavorite.h"
#include "launcherapplicationslistdbus.h"

#include "bamf-matcher.h"
#include "bamf-application.h"
#include "gconfitem-qml-wrapper.h"

#include <QStringList>
#include <QDir>
#include <QDebug>
#include <QDBusConnection>
#include <QFileInfo>

#define FAVORITES_KEY QString("/desktop/unity-2d/launcher/favorites")
#define DBUS_SERVICE_UNITY "com.canonical.Unity"
#define DBUS_SERVICE_LAUNCHER_ENTRY "com.canonical.Unity.LauncherEntry"
#define DBUS_SERVICE_LAUNCHER "com.canonical.Unity.Launcher"
#define DBUS_OBJECT_LAUNCHER "/com/canonical/Unity/Launcher"

LauncherApplicationsList::LauncherApplicationsList(QObject *parent) :
    QAbstractListModel(parent)
{
    m_favorites_list = new GConfItemQmlWrapper();
    m_favorites_list->setKey(FAVORITES_KEY);

    QDBusConnection session = QDBusConnection::sessionBus();
    /* FIXME: libunity will send out the Update signal for LauncherEntries
       only if it finds com.canonical.Unity on the bus, so let's just quickly
       register ourselves as Unity here. Should be moved somewhere else more proper */
    if (!session.registerService(DBUS_SERVICE_UNITY)) {
        qWarning() << "The name" << DBUS_SERVICE_UNITY << "is already taken on DBUS";
    } else {
        /* Set ourselves up to receive any Update signal coming from any
           LauncherEntry */
        session.connect(QString(), QString(),
                        DBUS_SERVICE_LAUNCHER_ENTRY, "Update",
                        this, SLOT(onRemoteEntryUpdated(QString,QMap<QString,QVariant>)));
    }

    if (!session.registerService(DBUS_SERVICE_LAUNCHER)) {
        qWarning() << "The name" << DBUS_SERVICE_LAUNCHER << "is already taken on DBUS";
    } else {
        /* Set ourselves up to receive a method call from Software Center asking us to add
           to favorites an application that is being installed and that the user requested
           to be added. */
        LauncherApplicationsListDBUS *dbusAdapter = new LauncherApplicationsListDBUS(this);
        if (!session.registerObject(DBUS_OBJECT_LAUNCHER, dbusAdapter,
                                    QDBusConnection::ExportAllSlots)) {
            qWarning() << "The object" << DBUS_OBJECT_LAUNCHER << "on" << DBUS_SERVICE_LAUNCHER
                       << "is already present on DBUS.";
        }
    }

    load();
}

void
LauncherApplicationsList::onRemoteEntryUpdated(QString applicationURI, QMap<QString, QVariant> properties)
{
    QString desktopFile;
    if (applicationURI.indexOf("application://") == 0) {
        desktopFile = applicationURI.mid(14);
    } else {
        qWarning() << "Ignoring update that didn't come from an application:// URI but from:" << applicationURI;
        return;
    }

    Q_FOREACH(LauncherApplication *application, m_applications) {
        if (QFileInfo(application->desktop_file()).fileName() == desktopFile) {
            application->updateOverlaysState(properties);
            return;
        }
    }

    qWarning() << "Application sent an update but we don't seem to have it in the launcher:" << applicationURI;
}

LauncherApplicationsList::~LauncherApplicationsList()
{
    qDeleteAll(m_applications);
    delete m_favorites_list;
}

QString
LauncherApplicationsList::favoriteFromDesktopFilePath(QString desktop_file)
{
    return QDir(desktop_file).dirName();
}

void
LauncherApplicationsList::insertApplication(LauncherApplication* application)
{
    /* Insert at the end of the list. */
    int index = m_applications.size();

    beginInsertRows(QModelIndex(), index, index);
    m_applications.insert(index, application);

    if (!application->desktop_file().isEmpty()) {
        m_applicationForDesktopFile.insert(application->desktop_file(), application);
    }
    endInsertRows();

    QObject::connect(application, SIGNAL(closed()), this, SLOT(onApplicationClosed()));
    QObject::connect(application, SIGNAL(stickyChanged(bool)), this, SLOT(onApplicationStickyChanged(bool)));
}

void
LauncherApplicationsList::removeApplication(LauncherApplication* application)
{
    int index = m_applications.indexOf(application);

    beginRemoveRows(QModelIndex(), index, index);
    m_applications.removeAt(index);
    m_applicationForDesktopFile.remove(application->desktop_file());
    endRemoveRows();

    delete application;
}

void LauncherApplicationsList::insertBamfApplication(BamfApplication* bamf_application)
{
    if (!bamf_application->user_visible()) {
        return;
    }

    LauncherApplication* application;

    QString desktop_file = bamf_application->desktop_file();
    if (m_applicationForDesktopFile.contains(desktop_file)) {
        /* A LauncherApplication with the same desktop file already exists */
        application = m_applicationForDesktopFile[desktop_file];
        application->setBamfApplication(bamf_application);
    } else {
        /* Create a new LauncherApplication and append it to the list */
        application = new LauncherApplication;
        application->setBamfApplication(bamf_application);
        insertApplication(application);
    }
}

void
LauncherApplicationsList::insertFavoriteApplication(QString desktop_file)
{
    if (m_applicationForDesktopFile.contains(desktop_file)) {
        return;
    }

    /* Create a new LauncherApplication */
    LauncherApplication* application = new LauncherApplication;
    application->setDesktopFile(desktop_file);

    /* If the desktop_file property is empty after setting it, it
       means glib couldn't load the desktop file (probably corrupted) */
    if (application->desktop_file().isEmpty()) {
        qWarning() << "Favorite application not added due to desktop file missing or corrupted ("
                   << desktop_file << ")";
        delete application;
    } else {
        insertApplication(application);
        application->setSticky(true);
    }
}

void
LauncherApplicationsList::insertWebFavorite(const QUrl& url)
{
    if (!url.isValid() || url.isRelative()) {
        qWarning() << "Invalid URL:" << url;
        return;
    }

    LauncherApplication* application = new LauncherApplication;
    WebFavorite* webfav = new WebFavorite(url, application);

    application->setDesktopFile(webfav->desktopFile());
    insertApplication(application);
    application->setSticky(true);
}

void
LauncherApplicationsList::load()
{
    /* Insert favorites */
    /* FIXME: migrate to GSettings, like unity. */
    QString desktop_file;
    QStringList favorites = m_favorites_list->getValue().toStringList();

    Q_FOREACH(QString favorite, favorites) {
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
LauncherApplicationsList::onBamfViewOpened(BamfView* bamf_view)
{
    /* Make sure bamf_view is in fact a BamfApplication */
    BamfApplication* bamf_application;
    bamf_application = dynamic_cast<BamfApplication*>(bamf_view);

    if (bamf_application == NULL) {
        return;
    }

    insertBamfApplication(bamf_application);
}

void LauncherApplicationsList::onApplicationClosed()
{
    LauncherApplication* application = static_cast<LauncherApplication*>(sender());

    if (!application->sticky() && !application->running()) {
        removeApplication(application);
    }
}

void
LauncherApplicationsList::onApplicationStickyChanged(bool sticky)
{
    LauncherApplication* application = static_cast<LauncherApplication*>(sender());

    writeFavoritesToGConf();

    if (!sticky && !application->running()) {
        removeApplication(application);
    }
}

void
LauncherApplicationsList::writeFavoritesToGConf()
{
    QStringList favorites;

    Q_FOREACH(LauncherApplication *application, m_applications) {
        QString desktop_file = application->desktop_file();
        if (application->sticky()) {
            favorites.append(favoriteFromDesktopFilePath(desktop_file));
        }
    }

    m_favorites_list->blockSignals(true);
    m_favorites_list->setValue(QVariant(favorites));
    m_favorites_list->blockSignals(false);
}

int
LauncherApplicationsList::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_applications.size();
}

QVariant
LauncherApplicationsList::data(const QModelIndex &index, int role) const
{
    Q_UNUSED(role);

    if (!index.isValid()) {
        return QVariant();
    }

    return QVariant::fromValue(m_applications.at(index.row()));
}

void
LauncherApplicationsList::move(int from, int to)
{
    QModelIndex parent;
    /* When moving an item down, the destination index needs to be incremented
       by one, as explained in the documentation:
       http://doc.qt.nokia.com/qabstractitemmodel.html#beginMoveRows */
    beginMoveRows(parent, from, from, parent, to + (to > from ? 1 : 0));
    m_applications.move(from, to);
    endMoveRows();

    if (m_applications[from]->sticky() || m_applications[to]->sticky()) {
        /* Update favorites only if at least one of the applications is a favorite */
        writeFavoritesToGConf();
    }
}

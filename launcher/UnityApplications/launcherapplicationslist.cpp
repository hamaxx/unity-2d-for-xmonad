#include "launcherapplication.h"
#include "launcherapplicationslist.h"

#include "bamf-matcher.h"
#include "bamf-application.h"

#include <QStringList>
#include <QDir>

#define FAVORITES_KEY QString("/desktop/unity/launcher/favorites/")

LauncherApplicationsList::LauncherApplicationsList(QObject *parent) :
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[0] = "application";
    setRoleNames(roles);

    m_favorites_list.setKey(FAVORITES_KEY + "favorites_list");

    load();
}

LauncherApplicationsList::~LauncherApplicationsList()
{
    QHash<QString, QLauncherApplication*>::iterator iter;
    for(iter=m_applications.begin(); iter!=m_applications.end(); iter++)
    {
        delete *iter;
    }
}

QString
LauncherApplicationsList::desktopFilePathFromFavorite(QString favorite_id)
{
    GConfItemQmlWrapper favorite;
    favorite.setKey(FAVORITES_KEY + favorite_id + "/desktop_file");
    return favorite.getValue().toString();
}

QString
LauncherApplicationsList::favoriteFromDesktopFilePath(QString desktop_file)
{
    return QString("app-") + QDir(desktop_file).dirName();
}


QLauncherApplication*
LauncherApplicationsList::insertApplication(QString desktop_file)
{
    if (m_applications.contains(desktop_file))
        return m_applications[desktop_file];

    /* Create a new QLauncherApplication */
    QLauncherApplication* application = new QLauncherApplication;
    application->setDesktopFile(desktop_file);

    beginInsertRows(QModelIndex(), m_applications.size(), m_applications.size());
    m_desktop_files.append(desktop_file);
    m_applications.insert(desktop_file, application);
    endInsertRows();

    QObject::connect(application, SIGNAL(stickyChanged(bool)), this, SLOT(onApplicationStickyChanged(bool)));

    return application;
}

void
LauncherApplicationsList::removeApplication(QString desktop_file)
{
    int index = m_desktop_files.indexOf(desktop_file);

    beginRemoveRows(QModelIndex(), index, index);
    m_desktop_files.removeAt(index);
    QLauncherApplication* application = m_applications.take(desktop_file);
    endRemoveRows();

    delete application;
}


void LauncherApplicationsList::insertBamfApplication(BamfApplication* bamf_application)
{
    if(!bamf_application->user_visible())
        return;

    QString desktop_file = bamf_application->desktop_file();
    QLauncherApplication* application = insertApplication(desktop_file);
    application->setBamfApplication(bamf_application);

    QObject::connect(application, SIGNAL(closed()), this, SLOT(onApplicationClosed()));
}

void
LauncherApplicationsList::insertFavoriteApplication(QString desktop_file)
{
    QLauncherApplication* application = insertApplication(desktop_file);
    application->setSticky(true);
}

void
LauncherApplicationsList::load()
{
    /* FIXME: applications should be sorted depending on their priority */

    /* Insert running applications from Bamf */
    BamfMatcher& matcher = BamfMatcher::get_default();
    BamfApplicationList* running_applications = matcher.running_applications();
    BamfApplication* bamf_application;

    for(int i=0; i<running_applications->size(); i++)
    {
        bamf_application = running_applications->at(i);
        insertBamfApplication(bamf_application);
    }

    QObject::connect(&matcher, SIGNAL(ViewOpened(BamfView*)), SLOT(onBamfViewOpened(BamfView*)));

    /* Insert favorites */
    QString desktop_file;
    QStringList favorites = m_favorites_list.getValue().toStringList();

    for(QStringList::iterator iter=favorites.begin(); iter!=favorites.end(); iter++)
    {
        desktop_file = desktopFilePathFromFavorite(*iter);
        insertFavoriteApplication(desktop_file);
    }
}

void
LauncherApplicationsList::onBamfViewOpened(BamfView* bamf_view)
{
    /* Make sure bamf_view is in fact a BamfApplication */
    BamfApplication* bamf_application;
    bamf_application = dynamic_cast<BamfApplication*>(bamf_view);

    if(bamf_application == NULL)
        return;

    insertBamfApplication(bamf_application);
}

void LauncherApplicationsList::onApplicationClosed()
{
    QLauncherApplication* application = static_cast<QLauncherApplication*>(sender());

    if (!application->sticky() && !application->running())
        removeApplication(application->desktop_file());
}

void
LauncherApplicationsList::onApplicationStickyChanged(bool sticky)
{
    QLauncherApplication* application = static_cast<QLauncherApplication*>(sender());

    if (sticky)
    {
        addApplicationToFavorites(application->desktop_file());
    }
    else
    {
        removeApplicationFromFavorites(application->desktop_file());
        if (!application->running())
            removeApplication(application->desktop_file());
    }
}

void
LauncherApplicationsList::addApplicationToFavorites(QString desktop_file)
{
    QString favorite_id = favoriteFromDesktopFilePath(desktop_file);

    /* Add the favorite id to the GConf list of favorites */
    QStringList favorites = m_favorites_list.getValue().toStringList();
    if (favorites.contains(favorite_id))
        return;
    favorites << favorite_id;
    m_favorites_list.blockSignals(true);
    m_favorites_list.setValue(QVariant(favorites));
    m_favorites_list.blockSignals(false);

    /* FIXME: storing these attributes in GConf should not be tied to adding
              application to the list of favorites but instead should happen
              in the QLauncherApplication itself whenever these values change.
    */
    /* Set GConf values corresponding to the favorite id for:
        - desktop file
        - type
        - priority
    */
    QLauncherApplication* application = m_applications[desktop_file];

    GConfItemQmlWrapper gconf_desktop_file;
    gconf_desktop_file.setKey(FAVORITES_KEY + favorite_id + "/desktop_file");
    gconf_desktop_file.setValue(QVariant(desktop_file));

    GConfItemQmlWrapper gconf_type;
    gconf_type.setKey(FAVORITES_KEY + favorite_id + "/type");
    gconf_type.setValue(QVariant(application->application_type()));

    GConfItemQmlWrapper gconf_priority;
    gconf_priority.setKey(FAVORITES_KEY + favorite_id + "/priority");
    /* FIXME: unity expects floats and not ints; it crashes at startup
              otherwise */
    gconf_priority.setValue(QVariant(double(application->priority())));
}

void
LauncherApplicationsList::removeApplicationFromFavorites(QString desktop_file)
{
    QStringList favorites = m_favorites_list.getValue().toStringList();

    for (QStringList::iterator i = favorites.begin(); i != favorites.end(); i++ )
    {
        QString current_desktop_file = desktopFilePathFromFavorite(*i);
        if (current_desktop_file == desktop_file)
        {
            favorites.erase(i);
            m_favorites_list.blockSignals(true);
            m_favorites_list.setValue(QVariant(favorites));
            m_favorites_list.blockSignals(false);
            /* The iterator 'i' is invalid but since we break off the loop
               nothing nasty happens. */
            break;
        }
    }
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

    if (!index.isValid())
        return QVariant();

    return QVariant::fromValue(m_applications[m_desktop_files.at(index.row())]);
}


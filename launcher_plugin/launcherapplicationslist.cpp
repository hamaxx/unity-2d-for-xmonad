#include "launcherapplication.h"
#include "launcherapplicationslist.h"
#include "bamf-matcher.h"
#include "bamf-application.h"
#include "libgq-fremantle/gconf/gconfitem.h"

LauncherApplicationsList::LauncherApplicationsList(QObject *parent) :
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[0] = "application";
    setRoleNames(roles);

    load();
}

LauncherApplicationsList::~LauncherApplicationsList()
{
    QList<QLauncherApplication*>::iterator iter;
    for(iter=m_applications.begin(); iter!=m_applications.end(); iter++)
    {
        delete *iter;
    }
}

QString
LauncherApplicationsList::desktopFilePathFromFavorite(QString favorite_id)
{
    return QString("/usr/share/applications/")+favorite_id+QString(".desktop");
}

void
LauncherApplicationsList::load()
{
    /* Assume m_applications is empty */

    /* FIXME: applications should be sorted depending on their priority */
    GConfItem gconf_favorites("/desktop/unity/launcher/favorites/favorites_list");
    QStringList favorites = gconf_favorites.value().toStringList();

    for(QStringList::iterator iter=favorites.begin(); iter!=favorites.end(); iter++)
        *iter = desktopFilePathFromFavorite(*iter);

    BamfMatcher& matcher = BamfMatcher::get_default();
    BamfApplicationList* running_applications = matcher.running_applications();
    BamfApplication* bamf_application;
    QLauncherApplication* application;

    for(int i=0; i<running_applications->size(); i++)
    {
        bamf_application = running_applications->at(i);
        favorites.removeAll(bamf_application->desktop_file());
        application = new QLauncherApplication;
        application->setBamfApplication(bamf_application);
        m_applications.append(application);
    }

    for(QStringList::iterator iter=favorites.begin(); iter!=favorites.end(); iter++)
    {
        application = new QLauncherApplication;
        application->setDesktopFile(*iter);
        m_applications.append(application);
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
    if (!index.isValid())
        return QVariant();

    return QVariant::fromValue(m_applications.at(index.row()));
}

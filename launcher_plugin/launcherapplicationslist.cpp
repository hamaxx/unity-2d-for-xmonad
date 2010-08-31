#include "launcherapplication.h"
#include "launcherapplicationslist.h"
#include "bamf-matcher.h"
#include "bamf-application.h"
#include "gconfitem-qml-wrapper.h"
#include <QStringList>

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
    /* FIXME: applications should be sorted depending on their priority */

    /* Assume m_applications is empty */

    GConfItemQmlWrapper gconf_favorites;
    gconf_favorites.setKey("/desktop/unity/launcher/favorites/favorites_list");
    QStringList favorites = gconf_favorites.getValue().toStringList();

    for(QStringList::iterator iter=favorites.begin(); iter!=favorites.end(); iter++)
        *iter = desktopFilePathFromFavorite(*iter);

    BamfMatcher& matcher = BamfMatcher::get_default();
    BamfApplicationList* running_applications = matcher.running_applications();
    BamfApplication* bamf_application;
    QLauncherApplication* application;

    /* Insert running applications from Bamf */
    for(int i=0; i<running_applications->size(); i++)
    {
        bamf_application = running_applications->at(i);
        favorites.removeAll(bamf_application->desktop_file());
        application = new QLauncherApplication;
        application->setBamfApplication(bamf_application);
        m_applications.append(application);
    }

    /* Insert remaining favorites that are not running */
    for(QStringList::iterator iter=favorites.begin(); iter!=favorites.end(); iter++)
    {
        application = new QLauncherApplication;
        application->setDesktopFile(*iter);
        m_applications.append(application);
    }

    QObject::connect(&matcher, SIGNAL(ViewOpened(BamfView*)), SLOT(onBamfViewOpened(BamfView*)));
}

void LauncherApplicationsList::insertBamfApplication(BamfApplication* bamf_application)
{
    QLauncherApplication* application;
    QList<QLauncherApplication*>::iterator iter;
    for(iter=m_applications.begin(); iter!=m_applications.end(); iter++)
    {
        application = *iter;
        if(application->desktop_file() == bamf_application->desktop_file())
        {
            /* There is an already existing QLauncherApplication corresponding
               to bamf_application */
            application->setBamfApplication(bamf_application);
            return;
        }
    }

    /* Create a new QLauncherApplication for bamf_application */
    application = new QLauncherApplication;
    application->setBamfApplication(bamf_application);

    beginInsertRows(QModelIndex(), m_applications.size(), m_applications.size());
    m_applications.append(application);
    endInsertRows();

    QObject::connect(application, SIGNAL(closed()), this, SLOT(onApplicationClosed()));
}

void LauncherApplicationsList::onApplicationClosed()
{
    QLauncherApplication* application = static_cast<QLauncherApplication*>(sender());
    int index = m_applications.indexOf(application);

    beginRemoveRows(QModelIndex(), index, index);
    m_applications.removeAt(index);
    endRemoveRows();

    delete application;
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

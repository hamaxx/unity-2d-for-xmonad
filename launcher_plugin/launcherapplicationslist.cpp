#include "launcherapplication.h"
#include "launcherapplicationslist.h"
#include "bamf-matcher.h"
#include "bamf-application.h"

LauncherApplicationsList::LauncherApplicationsList(QObject *parent) :
    QAbstractListModel(parent)
{
    QHash<int, QByteArray> roles;
    roles[0] = "application";
    setRoleNames(roles);

    reload();
}

void
LauncherApplicationsList::reload()
{
//        key: "/desktop/unity/launcher/favorites/favorites_list"

    BamfMatcher& matcher = BamfMatcher::get_default();
    BamfApplicationList* running_applications = matcher.running_applications();
    BamfApplication* bamf_application;
    QLauncherApplication* application;

    for(int i=0; i<running_applications->size(); i++)
    {
        bamf_application = running_applications->at(i);
        application = new QLauncherApplication;
        application->setBamfApplication(bamf_application);
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

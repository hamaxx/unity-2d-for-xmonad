#include "application.h"
#include "applicationslistdbus.h"
#include "applicationslist.h"
#include "applicationslistmanager.h"

ApplicationsListDBUS::ApplicationsListDBUS(QObject *parent) :
    QDBusAbstractAdaptor(parent)
{
}

void
ApplicationsListDBUS::AddLauncherItemFromPosition(QString title, QString icon,
                                                          int icon_x, int icon_y, int icon_size,
                                                          QString desktop_file, QString aptdaemon_task)
{
    Q_UNUSED(title)
    Q_UNUSED(icon_x)
    Q_UNUSED(icon_y)
    Q_UNUSED(icon_size)
    Q_UNUSED(aptdaemon_task)

    ApplicationsListManager* applicationsListManager = qobject_cast<ApplicationsListManager*>(parent());
    if (applicationsListManager != NULL && !desktop_file.isEmpty()) {
        Q_FOREACH(ApplicationsList *applicationsList, applicationsListManager->m_lists) {
            applicationsList->insertFavoriteApplication(desktop_file);
            Application *application = applicationsList->m_applicationForDesktopFile.value(desktop_file, NULL);
            if (application != NULL) {
                application->setIcon(icon);
                application->beginForceUrgent(1500);
            }
        }
    }
}

#include "applicationslistdbus.moc"

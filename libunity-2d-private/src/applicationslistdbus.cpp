#include "application.h"
#include "applicationslistdbus.h"
#include "applicationslist.h"

ApplicationsListDBUS::ApplicationsListDBUS(QObject *parent) :
    QDBusAbstractAdaptor(parent)
{
}

void
ApplicationsListDBUS::AddLauncherItemFromPosition(QString icon, QString title,
                                                          int icon_x, int icon_y, int icon_size,
                                                          QString desktop_file, QString aptdaemon_task)
{
    Q_UNUSED(icon)
    Q_UNUSED(title)
    Q_UNUSED(icon_x)
    Q_UNUSED(icon_y)
    Q_UNUSED(icon_size)
    Q_UNUSED(aptdaemon_task)

    ApplicationsList* applicationsList = qobject_cast<ApplicationsList*>(parent());
    if (applicationsList != NULL && !desktop_file.isEmpty()) {
        applicationsList->insertFavoriteApplication(desktop_file);
        Application *application = applicationsList->m_applicationForDesktopFile.value(desktop_file, NULL);
        if (application != NULL) {
            application->beginForceUrgent(1500);
        }
    }
}

#include "applicationslistdbus.moc"

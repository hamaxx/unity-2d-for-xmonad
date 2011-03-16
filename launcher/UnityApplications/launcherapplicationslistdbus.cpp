#include "launcherapplicationslistdbus.h"
#include "launcherapplicationslist.h"

LauncherApplicationsListDBUS::LauncherApplicationsListDBUS(QObject *parent) :
    QDBusAbstractAdaptor(parent)
{
}

void
LauncherApplicationsListDBUS::AddLauncherItemFromPosition(QString icon, QString title,
                                                          int icon_x, int icon_y, int icon_size,
                                                          QString desktop_file, QString aptdaemon_task)
{
    Q_UNUSED(icon)
    Q_UNUSED(title)
    Q_UNUSED(icon_x)
    Q_UNUSED(icon_y)
    Q_UNUSED(icon_size)
    Q_UNUSED(aptdaemon_task)

    LauncherApplicationsList* applicationsList = qobject_cast<LauncherApplicationsList*>(parent());
    if (applicationsList != NULL && !desktop_file.isEmpty()) {
        applicationsList->insertFavoriteApplication(desktop_file);
    }
}

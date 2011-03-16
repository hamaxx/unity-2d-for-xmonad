#ifndef LAUNCHERAPPLICATIONSLISTDBUS_H
#define LAUNCHERAPPLICATIONSLISTDBUS_H

#include <QObject>
#include <QtDBus/QDBusAbstractAdaptor>

class LauncherApplicationsListDBUS : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.Unity.Launcher")

public:
    explicit LauncherApplicationsListDBUS(QObject *parent = 0);

public Q_SLOTS:
    void AddLauncherItemFromPosition(QString icon, QString title,
                                     int icon_x, int icon_y, int icon_size,
                                     QString desktop_file, QString aptdaemon_task);

};

#endif // LAUNCHERAPPLICATIONSLISTDBUS_H

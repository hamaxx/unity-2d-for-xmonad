#ifndef APPLICATIONSLISTDBUS_H
#define APPLICATIONSLISTDBUS_H

#include <QObject>
#include <QtDBus/QDBusAbstractAdaptor>

class ApplicationsListDBUS : public QDBusAbstractAdaptor
{
    Q_OBJECT
    Q_CLASSINFO("D-Bus Interface", "com.canonical.Unity.Launcher")

public:
    explicit ApplicationsListDBUS(QObject *parent = 0);

public Q_SLOTS:
    void AddLauncherItemFromPosition(QString icon, QString title,
                                     int icon_x, int icon_y, int icon_size,
                                     QString desktop_file, QString aptdaemon_task);

};

#endif // APPLICATIONSLISTDBUS_H

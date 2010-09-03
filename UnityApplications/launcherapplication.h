#ifndef LAUNCHERAPPLICATION_H
#define LAUNCHERAPPLICATION_H

#include <gio/gdesktopappinfo.h>
#include <QObject>
#include <QUrl>
#include <QMetaType>
#include <QString>

#include "bamf-application.h"

class QLauncherApplication : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(bool urgent READ urgent NOTIFY urgentChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString icon READ icon NOTIFY iconChanged)
    Q_PROPERTY(QString application_type READ application_type NOTIFY applicationTypeChanged)

    Q_PROPERTY(QString desktop_file READ desktop_file WRITE setDesktopFile NOTIFY desktopFileChanged)
    Q_PROPERTY(bool priority READ priority NOTIFY priorityChanged)

public:
    QLauncherApplication(QObject *parent = 0);
    ~QLauncherApplication();

    /* getters */
    bool active() const;
    bool running() const;
    bool urgent() const;
    QString name() const;
    QString icon() const;
    QString application_type() const;
    QString desktop_file() const;
    int priority() const;

    /* setters */
    void setActive(bool active);
    void setDesktopFile(QString desktop_file);
    void setBamfApplication(BamfApplication *application);

    /* methods */
    Q_INVOKABLE QBool launch();
    Q_INVOKABLE void show();
    Q_INVOKABLE void close();


signals:
    void activeChanged(bool);
    void runningChanged(bool);
    void urgentChanged(bool);
    void nameChanged(QString);
    void iconChanged(QString);
    void applicationTypeChanged(QString);
    void desktopFileChanged(QString);
    void priorityChanged(int);

    void closed();

private slots:
    void onBamfApplicationClosed(bool running);
    void onDesktopFileChanged(QString desktop_file);

private:
    BamfApplication *m_application;
    GDesktopAppInfo *m_appInfo;
    int m_priority;
};

Q_DECLARE_METATYPE(QLauncherApplication*)


#endif // LAUNCHERAPPLICATION_H

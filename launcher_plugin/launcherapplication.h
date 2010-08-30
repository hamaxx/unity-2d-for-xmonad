#ifndef LAUNCHERAPPLICATION_H
#define LAUNCHERAPPLICATION_H

#include <gio/gdesktopappinfo.h>
#include <QObject>
#include <QUrl>
#include "bamf-application.h"

class QLauncherApplication : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString icon_name READ icon_name NOTIFY iconNameChanged)
    Q_PROPERTY(QString desktop_file READ desktop_file WRITE setDesktopFile NOTIFY desktopFileChanged)
    Q_PROPERTY(QString type READ type NOTIFY typeChanged)

public:
    QLauncherApplication(QObject *parent = 0);
    ~QLauncherApplication();

    /* getters */
    bool running() const;
    QString name() const;
    QString icon_name() const;
    QString desktop_file() const;
    QString type() const;

    /* setters */
    void setDesktopFile(QString desktop_file);

    /* methods */
    Q_INVOKABLE QBool launch();
    Q_INVOKABLE void show();
    Q_INVOKABLE void close();


signals:
    void runningChanged();
    void nameChanged();
    void iconNameChanged();
    void desktopFileChanged();
    void typeChanged();

public slots:

private:
    BamfApplication *m_application;
    GDesktopAppInfo *m_appInfo;
};

#endif // LAUNCHERAPPLICATION_H

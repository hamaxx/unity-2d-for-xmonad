#ifndef LAUNCHERAPPLICATION_H
#define LAUNCHERAPPLICATION_H

#include <launcher/launcher.h>
#include <QObject>
#include <QUrl>

class QLauncherApplication : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool running READ running NOTIFY runningChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString icon_name READ icon_name NOTIFY iconNameChanged)
    Q_PROPERTY(QUrl icon_path READ icon_path NOTIFY iconPathChanged)
    Q_PROPERTY(QString desktop_file READ desktop_file WRITE setDesktopFile NOTIFY desktopFileChanged)
public:
    QLauncherApplication(QObject *parent = 0);
    QLauncherApplication(QString desktop_file);
    ~QLauncherApplication();

    /* getters */
    bool running() const;
    QString name() const;
    QString icon_name() const;
    QUrl icon_path() const;
    QString desktop_file() const;

    /* setters */
    void setDesktopFile(QString desktop_file);

    /* methods */
    Q_INVOKABLE QBool launch();
    Q_INVOKABLE void show();
    Q_INVOKABLE void removeFromConnectionsList(QString id); // this method is original, ie. not a wrapper for liblauncher

    /* static notifiers */
    static void runningChangedStatic(GObject    *gobject __attribute__ ((unused)),
                                     GParamSpec *pspec __attribute__ ((unused)),
                                     QLauncherApplication *app) {app->runningChanged();}

    //static void desktopFileChangedStatic(QLauncherApplication* app) {app->desktopFileChanged();}
signals:
    void runningChanged();
    void nameChanged();
    void iconNameChanged();
    void iconPathChanged();
    void desktopFileChanged();

public slots:

private:
    LauncherApplication *m_application;
};

#endif // LAUNCHERAPPLICATION_H

#ifndef LAUNCHERAPPLICATION_H
#define LAUNCHERAPPLICATION_H

#include <gio/gdesktopappinfo.h>
#include <libwnck/libwnck.h>

#include "launcheritem.h"

#include <QObject>
#include <QUrl>
#include <QMetaType>
#include <QString>
#include <QTimer>

#include "bamf-application.h"

class LauncherApplication : public LauncherItem
{
    Q_OBJECT

    Q_PROPERTY(bool sticky READ sticky WRITE setSticky NOTIFY stickyChanged)
    Q_PROPERTY(QString application_type READ application_type NOTIFY applicationTypeChanged)
    Q_PROPERTY(QString desktop_file READ desktop_file WRITE setDesktopFile NOTIFY desktopFileChanged)
    Q_PROPERTY(int priority READ priority NOTIFY priorityChanged)
    Q_PROPERTY(bool has_visible_window READ has_visible_window NOTIFY hasVisibleWindowChanged)

public:
    LauncherApplication();
    LauncherApplication(const LauncherApplication& other);
    ~LauncherApplication();

    /* getters */
    virtual bool active() const;
    virtual bool running() const;
    virtual bool urgent() const;
    bool sticky() const;
    virtual QString name() const;
    virtual QString icon() const;
    QString application_type() const;
    QString desktop_file() const;
    int priority() const;
    virtual bool launching() const;
    bool has_visible_window() const;

    /* setters */
    void setSticky(bool sticky);
    void setDesktopFile(QString desktop_file);
    void setBamfApplication(BamfApplication *application);

    /* methods */
    Q_INVOKABLE virtual void activate();
    Q_INVOKABLE void close();
    Q_INVOKABLE void expose();
    Q_INVOKABLE void setIconGeometry(int x, int y, int width, int height, uint xid=0);

    Q_INVOKABLE virtual void createMenuActions();

signals:
    void stickyChanged(bool);
    void applicationTypeChanged(QString);
    void desktopFileChanged(QString);
    void priorityChanged(int);
    void hasVisibleWindowChanged(bool);

    void closed();

    void windowAdded(uint xid);

private slots:
    void onBamfApplicationClosed(bool running);
    void onLaunchingTimeouted();
    void updateHasVisibleWindow();

    bool launch();
    void show();

    /* Contextual menu callbacks */
    void onKeepTriggered();
    void onQuitTriggered();

    void onWindowAdded(BamfWindow*);

private:
    BamfApplication *m_application;
    GDesktopAppInfo *m_appInfo;
    bool m_sticky;
    int m_priority;
    QTimer m_launching_timer;
    bool m_has_visible_window;

    void moveViewportToWindow(WnckWindow* window);
    void updateBamfApplicationDependentProperties();
};

Q_DECLARE_METATYPE(LauncherApplication*)

#endif // LAUNCHERAPPLICATION_H


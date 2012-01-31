#ifndef DESKTOPINFO_H
#define DESKTOPINFO_H

#include <QObject>
#include <QRect>

#include "workspacesinfo.h"

typedef void* gpointer;
typedef struct _WnckScreen WnckScreen;
typedef struct _WnckWindow WnckWindow;

class DesktopInfo : public QObject
{
    Q_OBJECT

    Q_ENUMS(WorkspacesOrientation)
    Q_ENUMS(WorkspacesCorner)

    Q_PROPERTY(WorkspacesInfo *workspaces READ workspaces NOTIFY workspacesChanged)
    Q_PROPERTY(unsigned int activeWindow READ activeWindow NOTIFY activeWindowChanged)
    Q_PROPERTY(bool isCompositingManagerRunning READ isCompositingManagerRunning
                                                NOTIFY isCompositingManagerRunningChanged)

public:
    static DesktopInfo* instance();

    /* Getters */
    WorkspacesInfo *workspaces() { return &m_workspacesInfo; }
    unsigned int activeWindow() const { return m_activeWindow; }
    bool isCompositingManagerRunning() const;

Q_SIGNALS:
    void activeWindowChanged(unsigned int activeWindow);
    void workspacesChanged(WorkspacesInfo *workspaces);
    void isCompositingManagerRunningChanged(bool);

private:
    explicit DesktopInfo(QObject *parent = 0);
    void updateActiveWindow(WnckScreen *screen);

    static void onActiveWindowChanged(WnckScreen *screen,
                                      WnckWindow *previously_active_window,
                                      gpointer    user_data);

private:
    WorkspacesInfo m_workspacesInfo;
    unsigned int m_activeWindow;
};

#endif // DESKTOPINFO_H

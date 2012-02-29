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

    Q_PROPERTY(WorkspacesInfo *workspaces READ workspaces NOTIFY workspacesChanged)
    Q_PROPERTY(bool isCompositingManagerRunning READ isCompositingManagerRunning
                                                NOTIFY isCompositingManagerRunningChanged)

public:
    static DesktopInfo* instance();

    /* Getters */
    WorkspacesInfo *workspaces() { return &m_workspacesInfo; }
    bool isCompositingManagerRunning() const;

Q_SIGNALS:
    void workspacesChanged(WorkspacesInfo *workspaces);
    void isCompositingManagerRunningChanged(bool);

private:
    explicit DesktopInfo(QObject *parent = 0);

private:
    WorkspacesInfo m_workspacesInfo;
};

#endif // DESKTOPINFO_H

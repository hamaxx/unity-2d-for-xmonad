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
    Q_PROPERTY(int totalWidth READ totalWidth NOTIFY totalWidthChanged)

public:
    static DesktopInfo* instance();

    /* Getters */
    WorkspacesInfo *workspaces() { return &m_workspacesInfo; }
    bool isCompositingManagerRunning() const;
    int totalWidth() const;

Q_SIGNALS:
    void workspacesChanged(WorkspacesInfo *workspaces);
    void isCompositingManagerRunningChanged(bool);
    void totalWidthChanged() const;

private:
    explicit DesktopInfo(QObject *parent = 0);

private:
    WorkspacesInfo m_workspacesInfo;
};

#endif // DESKTOPINFO_H

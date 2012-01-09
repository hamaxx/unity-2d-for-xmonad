#ifndef SCREENINFO_H
#define SCREENINFO_H

#include <QObject>
#include <QRect>

#include "workspacesinfo.h"

typedef void* gpointer;
typedef struct _WnckScreen WnckScreen;
typedef struct _WnckWindow WnckWindow;

class ScreenInfo : public QObject
{
    Q_OBJECT

    Q_ENUMS(WorkspacesOrientation)
    Q_ENUMS(WorkspacesCorner)

    Q_PROPERTY(WorkspacesInfo *workspaces READ workspaces NOTIFY workspacesChanged)
    Q_PROPERTY(unsigned int activeWindow READ activeWindow NOTIFY activeWindowChanged)
    Q_PROPERTY(QRect geometry READ geometry NOTIFY geometryChanged)
    Q_PROPERTY(QRect availableGeometry READ availableGeometry NOTIFY availableGeometryChanged)
    Q_PROPERTY(QRect panelsFreeGeometry READ panelsFreeGeometry NOTIFY panelsFreeGeometryChanged)
    Q_PROPERTY(bool isCompositingManagerRunning READ isCompositingManagerRunning
                                                NOTIFY isCompositingManagerRunningChanged)

public:
    static ScreenInfo* instance();

    /* Getters */
    WorkspacesInfo *workspaces() { return &m_workspacesInfo; }
    unsigned int activeWindow() const { return m_activeWindow; }
    QRect availableGeometry() const;
    QRect panelsFreeGeometry() const;
    QRect geometry() const;
    bool isCompositingManagerRunning() const;

Q_SIGNALS:
    void activeWindowChanged(unsigned int activeWindow);
    void geometryChanged(QRect geometry);
    void availableGeometryChanged(QRect availableGeometry);
    void panelsFreeGeometryChanged(QRect panelsFreeGeometry);
    void workspacesChanged(WorkspacesInfo *workspaces);
    void isCompositingManagerRunningChanged(bool);

private:
    explicit ScreenInfo(QObject *parent = 0);
    void updateActiveWindow(WnckScreen *screen);

    static void onActiveWindowChanged(WnckScreen *screen,
                                      WnckWindow *previously_active_window,
                                      gpointer    user_data);
private Q_SLOTS:
    void updateGeometry(int screen);
    void updateAvailableGeometry(int screen);
    void onLauncherThicknessChanged();

private:
    WorkspacesInfo m_workspacesInfo;
    unsigned int m_activeWindow;
};

#endif // SCREENINFO_H

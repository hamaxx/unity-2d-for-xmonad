#ifndef SCREENINFO_H
#define SCREENINFO_H

#include <QObject>
#include <QRect>

typedef void* gpointer;
typedef struct _WnckScreen WnckScreen;
typedef struct _WnckWindow WnckWindow;
typedef unsigned long Atom;

class ScreenInfo : public QObject
{
    Q_OBJECT

    Q_ENUMS(Orientation)
    Q_ENUMS(Corner)

    Q_PROPERTY(int workspaces READ workspaces NOTIFY workspacesChanged)
    Q_PROPERTY(int currentWorkspace READ currentWorkspace NOTIFY currentWorkspaceChanged)
    Q_PROPERTY(int rows READ rows NOTIFY rowsChanged)
    Q_PROPERTY(int columns READ columns NOTIFY columnsChanged)
    Q_PROPERTY(Orientation orientation READ orientation NOTIFY orientationChanged)
    Q_PROPERTY(Corner startingCorner READ startingCorner NOTIFY startingCornerChanged)
    Q_PROPERTY(unsigned int activeWindow READ activeWindow NOTIFY activeWindowChanged)
    Q_PROPERTY(QRect geometry READ geometry NOTIFY geometryChanged)
    Q_PROPERTY(QRect availableGeometry READ availableGeometry NOTIFY availableGeometryChanged)

public:
    enum Orientation {
        OrientationHorizontal,
        OrientationVertical
    };
    enum Corner {
        CornerTopLeft,
        CornerTopRight,
        CornerBottomRight,
        CornerBottomLeft
    };

    static ScreenInfo* instance();

    Q_INVOKABLE void activateWorkspace(int workspaceNumber);

    /* The following two methods don't strictly belong to a "screen" class
       logically. It would be perhaps more appropriate to make an
       "utility" class to group them, but this will do for now. */
    Q_INVOKABLE QString desktopFileForApplication(int applicationId);
    Q_INVOKABLE QString currentTime();

    /* Getters */
    int workspaces() const { return m_workspaces; }
    int currentWorkspace() const { return m_currentWorkspace; }
    int rows() const { return m_rows; }
    int columns() const { return m_columns; }
    Orientation orientation() const { return m_orientation; }
    Corner startingCorner() const { return m_startingCorner; }
    unsigned int activeWindow() const { return m_activeWindow; }
    QRect availableGeometry() const;
    QRect geometry() const;

signals:
    void workspacesChanged(int workspaces);
    void currentWorkspaceChanged(int currentWorkspace);
    void rowsChanged(int rows);
    void columnsChanged(int columns);
    void orientationChanged(Orientation orientation);
    void startingCornerChanged(Corner startingCorner);
    void activeWindowChanged(unsigned int activeWindow);
    void geometryChanged(QRect geometry);
    void availableGeometryChanged(QRect availableGeometry);

private:
    explicit ScreenInfo(QObject *parent = 0);
    static bool globalEventFilter(void* message);

    void updateWorkspaceGeometry();
    void updateCurrentWorkspace();
    void updateActiveWindow(WnckScreen *screen);

    unsigned long * getX11IntProperty(Atom property, long length);
    static void internX11Atoms();
    bool readWorkspacesConfiguration(int *rows);
    bool setWorkspacesGeometryFromConfiguration();

    static void onActiveWindowChanged(WnckScreen *screen,
                                      WnckWindow *previously_active_window,
                                      gpointer    user_data);
private slots:
    void updateGeometry(int screen);
    void updateAvailableGeometry(int screen);

private:
    int m_workspaces;
    int m_currentWorkspace;
    int m_rows;
    int m_columns;
    Orientation m_orientation;
    Corner m_startingCorner;
    unsigned int m_activeWindow;
};

#endif // SCREENINFO_H

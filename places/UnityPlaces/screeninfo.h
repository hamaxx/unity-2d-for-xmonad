#ifndef SCREENINFO_H
#define SCREENINFO_H

#include <QObject>
#include <X11/X.h>

typedef void* gpointer;
typedef struct _WnckWorkspace WnckWorkspace;
typedef struct _WnckScreen WnckScreen;

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
    int workspaces() const { return m_workspaces; }
    int currentWorkspace() const { return m_currentWorkspace; }
    int rows() const { return m_rows; }
    int columns() const { return m_columns; }
    Orientation orientation() const { return m_orientation; }
    Corner startingCorner() const { return m_startingCorner; }

signals:
    void workspacesChanged(int workspaces);
    void currentWorkspaceChanged(int currentWorkspace);
    void rowsChanged(int rows);
    void columnsChanged(int columns);
    void orientationChanged(Orientation orientation);
    void startingCornerChanged(Corner startingCorner);

private:
    explicit ScreenInfo(QObject *parent = 0);
    static bool globalEventFilter(void* message);

    void updateWorkspaceGeometry();
    void updateCurrentWorkspace();

    unsigned long * getX11IntProperty(Atom property, long length);
    static void internX11Atoms();

private:
    int m_workspaces;
    int m_currentWorkspace;
    int m_rows;
    int m_columns;
    Orientation m_orientation;
    Corner m_startingCorner;
};

#endif // SCREENINFO_H

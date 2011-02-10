#ifndef WORKSPACESINFO_H
#define WORKSPACESINFO_H

#include <QObject>

typedef unsigned long Atom;

class WorkspacesInfo : public QObject
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int current READ current NOTIFY currentChanged)
    Q_PROPERTY(int rows READ rows NOTIFY rowsChanged)
    Q_PROPERTY(int columns READ columns NOTIFY columnsChanged)
    Q_PROPERTY(Orientation orientation READ orientation NOTIFY orientationChanged)
    Q_PROPERTY(Corner startingCorner READ startingCorner NOTIFY startingCornerChanged)

 public:
    explicit WorkspacesInfo(QObject *parent = 0);

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

    /* Getters */
    int count() const { return m_count; }
    int current() const { return m_current; }
    int rows() const { return m_rows; }
    int columns() const { return m_columns; }
    Orientation orientation() const { return m_orientation; }
    Corner startingCorner() const { return m_startingCorner; }

    /* Invokables */
    Q_INVOKABLE bool changeCurrent(int newWorkspace);

Q_SIGNALS:
    void countChanged(int count);
    void currentChanged(int current);
    void rowsChanged(int rows);
    void columnsChanged(int columns);
    void orientationChanged(Orientation orientation);
    void startingCornerChanged(Corner startingCorner);

private:
    static bool globalEventFilter(void* message);
    static void internX11Atoms();
    void updateWorkspaceGeometry();
    void updateCurrentWorkspace();
    unsigned long * getX11IntProperty(Atom property, long length);

private:
    int m_count;
    int m_current;
    int m_rows;
    int m_columns;
    Orientation m_orientation;
    Corner m_startingCorner;
};

#endif // WORKSPACESINFO_H

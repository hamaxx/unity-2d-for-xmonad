#include "workspacesinfo.h"
#include "desktopinfo.h"
#include "signalwaiter.h"
#include <debug_p.h>

extern "C" {
#include <libwnck/libwnck.h>
}

#include <QAbstractEventDispatcher>
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <math.h>

QAbstractEventDispatcher::EventFilter oldEventFilter;
Atom _NET_DESKTOP_LAYOUT;
Atom _NET_NUMBER_OF_DESKTOPS;
Atom _NET_CURRENT_DESKTOP;

WorkspacesInfo::WorkspacesInfo(QObject *parent) :
    QObject(parent),
    m_count(0),
    m_current(0),
    m_rows(0),
    m_columns(0),
    m_orientation(OrientationHorizontal),
    m_startingCorner(CornerTopLeft)
{
    WorkspacesInfo::internX11Atoms();

    /* Setup an low-level event filter so that we can get X11 events directly,
       then ask X11 to notify us of property changes on the root window. This
       will include notiication on workspace geometry changes.
    */
    QAbstractEventDispatcher *dispatcher = QAbstractEventDispatcher::instance();
    oldEventFilter = dispatcher->setEventFilter(WorkspacesInfo::globalEventFilter);
    XSelectInput(QX11Info::display(), QX11Info::appRootWindow(), PropertyChangeMask);

    updateWorkspaceGeometry();
    updateCurrentWorkspace();
}

/* X11 Atoms never change, so let's just avoid repeating these calls more than once */
void WorkspacesInfo::internX11Atoms()
{
    _NET_DESKTOP_LAYOUT = XInternAtom(QX11Info::display(),
                                      "_NET_DESKTOP_LAYOUT",
                                      False);
    _NET_NUMBER_OF_DESKTOPS = XInternAtom(QX11Info::display(),
                                          "_NET_NUMBER_OF_DESKTOPS",
                                          False);
    _NET_CURRENT_DESKTOP = XInternAtom(QX11Info::display(),
                                       "_NET_CURRENT_DESKTOP",
                                       False);
}

bool WorkspacesInfo::globalEventFilter(void* message)
{
    /* Always call the old event handler before we do our thing, and
       make sure to always return what it did return, as we don't really
       "handle" any event, we just monitor them */
    bool ret;
    ret = (oldEventFilter != NULL) ? oldEventFilter(message) : false;

    XEvent* event = static_cast<XEvent*>(message);
    if (event->type != PropertyNotify) {
        return ret;
    }

    XPropertyEvent* notify = (XPropertyEvent*) event;

    if (notify->atom == _NET_DESKTOP_LAYOUT ||
        notify->atom == _NET_NUMBER_OF_DESKTOPS) {
        DesktopInfo::instance()->workspaces()->updateWorkspaceGeometry();
    } else if (notify->atom == _NET_CURRENT_DESKTOP) {
        DesktopInfo::instance()->workspaces()->updateCurrentWorkspace();
    }

    return ret;
}

void WorkspacesInfo::updateWorkspaceGeometry()
{
    int workspaceCount = 0;
    Orientation orientation = WorkspacesInfo::OrientationHorizontal;
    int columns = 0;
    int rows = 0;
    int squareColumns;
    int squareRows;
    Corner startingCorner = WorkspacesInfo::CornerTopLeft;
    bool XWorkspaceCountNeedsUpdate = false;
    bool XLayoutNeedsUpdate = false;

    XWorkspaceCountNeedsUpdate = !getWorkspaceCountFromX(workspaceCount);
    XLayoutNeedsUpdate = !getWorkspaceLayoutFromX(orientation, columns, rows, startingCorner);

    /* If the number of workspaces was not set or set to 0 fallback to a default
       number of 4 workspaces. */
    if (workspaceCount == 0) {
        workspaceCount = 4;
        XWorkspaceCountNeedsUpdate = true;
    }

    /* Override the number of rows and columns possibly set by an external pager.
       The number of rows and columns is computed from the number of workspaces
       to make the layout square.

       We override external pagers because differentiating an external pager from
       oneself is hard. Example case where it matters:
       Starting with 9 workspaces, we decide on 3 rows and 3 columns. When the
       number of workspaces is then changed dynamically to 4 workspaces, without
       override, we would layout the 4 workspaces in a 3x3 grid.
    */
    squareColumns = ceil(sqrt((float)workspaceCount));
    squareRows = ceil((float) workspaceCount / (float) squareColumns);

    if (squareColumns != columns || squareRows != rows) {
        columns = squareColumns;
        rows = squareRows;
        XLayoutNeedsUpdate = true;
    }

    if (XWorkspaceCountNeedsUpdate) {
        setWorkspaceCountToX(workspaceCount);
    }

    if (XLayoutNeedsUpdate) {
        setWorkspaceLayoutToX(orientation, columns, rows, startingCorner);
    }

    updateWorkspaceGeometryProperties(workspaceCount, orientation, columns, rows, startingCorner);
}

bool WorkspacesInfo::getWorkspaceCountFromX(int& workspaceCount)
{
    unsigned long *result;
    result = getX11IntProperty(_NET_NUMBER_OF_DESKTOPS, 1);
    if (result != NULL) {
        workspaceCount = result[0];
        XFree(result);
        return true;
    } else {
        return false;
    }
}

bool WorkspacesInfo::getWorkspaceLayoutFromX(Orientation& orientation, int& columns, int& rows, Corner& startingCorner)
{
    unsigned long *result;
    result = getX11IntProperty(_NET_DESKTOP_LAYOUT, 4);
    if (result != NULL) {
        orientation = (Orientation) result[0];
        columns = result[1];
        rows = result[2];
        startingCorner = (Corner) result[3];
        XFree(result);
        return true;
    } else {
        return false;
    }
}

void WorkspacesInfo::setWorkspaceCountToX(int workspaceCount)
{
    unsigned long values[1];
    values[0] = workspaceCount;
    setX11IntProperty(_NET_NUMBER_OF_DESKTOPS, (unsigned char*)values, 1);
}

void WorkspacesInfo::setWorkspaceLayoutToX(Orientation orientation, int columns, int rows, Corner startingCorner)
{
    unsigned long values[4];
    values[0] = orientation;
    values[1] = columns;
    values[2] = rows;
    values[3] = startingCorner;
    setX11IntProperty(_NET_DESKTOP_LAYOUT, (unsigned char*)values, 4);
}

void WorkspacesInfo::updateWorkspaceGeometryProperties(int workspaceCount, Orientation orientation, int columns, int rows, Corner startingCorner)
{
    if (m_count != workspaceCount) {
        m_count = workspaceCount;
        Q_EMIT countChanged(m_count);
    }
    if (m_rows != rows) {
        m_rows = rows;
        Q_EMIT rowsChanged(m_rows);
    }
    if (m_columns != columns) {
        m_columns = columns;
        Q_EMIT columnsChanged(m_columns);
    }
    if (m_orientation != orientation) {
        m_orientation = orientation;
        Q_EMIT orientationChanged(m_orientation);
    }
    if (m_startingCorner != startingCorner) {
        m_startingCorner = startingCorner;
        Q_EMIT startingCornerChanged(m_startingCorner);
    }
}

void WorkspacesInfo::updateCurrentWorkspace()
{
    int currentWorkspace;
    unsigned long *result;
    result = getX11IntProperty(_NET_CURRENT_DESKTOP, 1);
    if (result == NULL) {
        currentWorkspace = 0;
    } else {
        currentWorkspace = result[0];
    }
    XFree(result);

    if (m_current != currentWorkspace) {
        m_current = currentWorkspace;
        Q_EMIT currentChanged(m_current);
    }
}

/* FIXME: this will not work with wm using large desktops and viewports
   to implement their workspaces.*/
bool WorkspacesInfo::changeCurrent(int newWorkspace)
{
    WnckScreen *screen = wnck_screen_get_default();
    WnckWorkspace *workspace = wnck_screen_get_workspace(screen, newWorkspace);
    if (workspace == NULL) {
        UQ_WARNING << "Requested activation workspace" << newWorkspace << " but it does not exist.";
        return false;
    }

    if (newWorkspace == current()) {
        return true;
    }

    /* This function will ask the WM to change workspace. However we have no way to know
       if it succeeds or fails. To know that we can only wait to be notified of the workspace
       to actually change, or decide we waited too much and proceed anyway */
    wnck_workspace_activate(workspace, CurrentTime);
    return SignalWaiter().waitForSignal(this, SIGNAL(currentChanged(int)), 50);
}

/* Helper function to read the value of an X11 window property of integer type of the
   length specified by by length and with name specified by property.
   Please notice that you have to free the returned value with XFree in case of
   success. */
unsigned long * WorkspacesInfo::getX11IntProperty(Atom property, long length)
{
    Atom type_ret;
    int format_ret;
    unsigned long nitems_ret;
    unsigned long bytes_after_ret;
    unsigned char *value_ret;
    int result;

    result = XGetWindowProperty(QX11Info::display(), QX11Info::appRootWindow(),
                                property,
                                0, length, False, XA_CARDINAL,
                                &type_ret, &format_ret, &nitems_ret,
                                &bytes_after_ret, &value_ret);
    if (result == Success) {
        if (type_ret != None) {
            return (unsigned long *) value_ret;
        }
    }

    return NULL;
}

/* Helper function to write the value of X11 window property of integer type of
   the length specified. */
void WorkspacesInfo::setX11IntProperty(Atom property, unsigned char *data, long length)
{
    XChangeProperty(QX11Info::display(), QX11Info::appRootWindow(),
                    property,
                    XA_CARDINAL, 32,
                    PropModeReplace,
                    data, length);
}

#include "workspacesinfo.moc"

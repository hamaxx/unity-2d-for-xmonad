#include "workspacesinfo.h"
#include "screeninfo.h"
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
        ScreenInfo::instance()->workspaces()->updateWorkspaceGeometry();
    } else if (notify->atom == _NET_CURRENT_DESKTOP) {
        ScreenInfo::instance()->workspaces()->updateCurrentWorkspace();
    }

    return ret;
}

void WorkspacesInfo::updateWorkspaceGeometry()
{
    int workspaceCount;
    int rows;
    int columns;
    Orientation orientation;
    Corner startingCorner;
    unsigned long *result;

    /* First obtain the number of workspaces, that will be needed to
       also calculate some properties of the layout which could be missing
       from the property we will retrieve after this one */
    result = getX11IntProperty(_NET_NUMBER_OF_DESKTOPS, 1);
    if (result == NULL) {
        workspaceCount = 4;
    } else {
        workspaceCount = result[0];
    }
    XFree(result);

    /* Then ask X11 the layout of the workspaces. */
    result = getX11IntProperty(_NET_DESKTOP_LAYOUT, 4);
    if (result != NULL) {
        /* If we read the values correctly (some pager or the user set them)
           then just use them. */
        orientation = (Orientation) result[0];
        columns = result[1];
        rows = result[2];
        startingCorner = (Corner) result[3];

        /* Perform some sanity checks, since it's possible according to
           the spec for rows or colums to be zero, and we are supposed to
           calculate these values ourselves by using the total count
        */
        if (rows == 0 && columns == 0) {
            rows = 2;
            columns = workspaceCount;
        } else {
            if (rows == 0) {
                rows = ceil((float) workspaceCount / (float) columns);
            } else if (columns == 0) {
                columns = ceil((float) workspaceCount / (float) rows);
            }
        }

        XFree(result);
    } else {
        /* In this property does not exist (as is the case if you
           don't login into the regular gnome session before unity-2d),
           just fallback on reasonable defaults. */
        rows = 2;
        columns = ceil((float) workspaceCount / (float) rows);
        orientation = WorkspacesInfo::OrientationHorizontal;
        startingCorner = WorkspacesInfo::CornerTopLeft;
    }

    /* Notify of changes, if any */
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

#include "workspacesinfo.moc"

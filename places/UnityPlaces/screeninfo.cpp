#include <libwnck/screen.h>
#include <libwnck/window.h>
#include <libwnck/workspace.h>

#include <glib-2.0/glib.h>

//#include "bamf-matcher.h"
//#include "bamf-application.h"
//#include "bamf-window.h"

#include "screeninfo.h"

#include <QDebug>
#include <QAbstractEventDispatcher>
#include <QX11Info>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#include <math.h>

QAbstractEventDispatcher::EventFilter oldEventFilter;
Atom _NET_DESKTOP_LAYOUT;
Atom _NET_NUMBER_OF_DESKTOPS;
Atom _NET_CURRENT_DESKTOP;

ScreenInfo::ScreenInfo(QObject *parent) :
    QObject(parent)
{
    ScreenInfo::internX11Atoms();

    /* Setup an low-level event filter so that we can get X11 events directly,
       then ask X11 to notify us of property changes on the root window. This
       will include notiication on workspace geometry changes.
    */
    QAbstractEventDispatcher *dispatcher = QAbstractEventDispatcher::instance();
    oldEventFilter = dispatcher->setEventFilter(ScreenInfo::globalEventFilter);
    XSelectInput(QX11Info::display(), QX11Info::appRootWindow(), PropertyChangeMask);

    updateWorkspaceGeometry();
    updateCurrentWorkspace();
}

ScreenInfo* ScreenInfo::instance()
{
    static ScreenInfo* singleton = new ScreenInfo();
    return singleton;
}

bool ScreenInfo::globalEventFilter(void* message)
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
        ScreenInfo::instance()->updateWorkspaceGeometry();
    } else if (notify->atom == _NET_CURRENT_DESKTOP) {
        ScreenInfo::instance()->updateCurrentWorkspace();
    }

    return ret;
}

unsigned long * ScreenInfo::getX11IntProperty(Atom property, long length)
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

/* X11 Atoms never change, so let's just avoid repeating these calls more than once */
void ScreenInfo::internX11Atoms()
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

void ScreenInfo::updateWorkspaceGeometry()
{
    int workspaces;
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
        workspaces = 1;
    } else {
        workspaces = result[0];
    }
    XFree(result);

    /* Then ask X11 the layout of the workspaces. */
    result = getX11IntProperty(_NET_DESKTOP_LAYOUT, 4);
    if (result == NULL) {
            /* In case of errors, default to a 1-row layout with "natural" orientation
               and starting corner (natural for westerners, at least)
               Note that if no pager has been started (which is the case with the
               default unity-2d session, then this property will not exist.
            */
            orientation = ScreenInfo::OrientationHorizontal;
            startingCorner = ScreenInfo::CornerTopLeft;
            rows = 1;
            columns = workspaces;
    } else {
        orientation = (Orientation) result[0];
        columns = result[1];
        rows = result[2];
        startingCorner = (Corner) result[3];

        /* Perform some sanity checks, since it's possible according to
           the spec for rows or colums to be zero, and we are supposed to
           calculate these values ourselves by using the total count
        */
        if (rows == 0 && columns == 0) {
            rows = 1;
            columns = workspaces;
        } else {
            if (rows == 0) {
                rows = ceil((float) workspaces / (float) columns);
            } else if (columns == 0) {
                columns = ceil((float) workspaces / (float) rows);
            }
        }

        XFree(result);
    }

    /* Notify of changes, if any */
    if (m_workspaces != workspaces) {
        m_workspaces = workspaces;
        Q_EMIT workspacesChanged(m_workspaces);
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

void ScreenInfo::updateCurrentWorkspace()
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

    if (m_currentWorkspace != currentWorkspace) {
        m_currentWorkspace = currentWorkspace;
        Q_EMIT currentWorkspaceChanged(m_currentWorkspace);
    }
}

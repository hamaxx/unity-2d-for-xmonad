#include <libwnck/screen.h>
#include <libwnck/window.h>
#include <libwnck/workspace.h>

#include <glib-2.0/glib.h>

#include "bamf-matcher.h"
#include "bamf-application.h"
#include "bamf-window.h"

#include "gconfitem-qml-wrapper.h"

#include "screeninfo.h"

#include <QDebug>
#include <QAbstractEventDispatcher>
#include <QX11Info>
#include <QApplication>
#include <QDesktopWidget>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/X.h>

#include <math.h>
#include <time.h>


QAbstractEventDispatcher::EventFilter oldEventFilter;
Atom _NET_DESKTOP_LAYOUT;
Atom _NET_NUMBER_OF_DESKTOPS;
Atom _NET_CURRENT_DESKTOP;

#define GNOME_WORKSPACES_ROWS_KEY "/apps/panel/applets/workspace_switcher_screen%1/prefs/num_rows"

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

    WnckScreen *screen = wnck_screen_get_default();
    g_signal_connect(G_OBJECT(screen), "active-window-changed",
                     G_CALLBACK(ScreenInfo::onActiveWindowChanged), NULL);

    updateActiveWindow(screen);

    connect(QApplication::desktop(), SIGNAL(resized(int)),
                                     SLOT(updateGeometry(int)));
    connect(QApplication::desktop(), SIGNAL(workAreaResized(int)),
                                     SLOT(updateAvailableGeometry(int)));
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

/* Read the workspace configuration.
   For now, we only read the number of rows from the gconf key of
   gnome's panel workspaces applet.
   FIXME: check what compiz uses and read it when compiz is running */
bool ScreenInfo::readWorkspacesConfiguration(int *rows)
{
    GConfItemQmlWrapper item;
    QString key = QString(GNOME_WORKSPACES_ROWS_KEY);
    item.setKey(key.arg(QString::number(QX11Info::appScreen())));
    bool valid;
    *rows = item.getValue().toInt(&valid);
    if (!valid) {
        *rows = 0;
    }
    return valid;
}

bool ScreenInfo::setWorkspacesGeometryFromConfiguration()
{
    int rows;
    bool success = readWorkspacesConfiguration(&rows);
    if (success) {
        /* According to the WNCK documentation, it should be possible to
           set both columns and rows. However WNCK throws an error if one
           of the two isn't zero. So let's just pass the number of rows
           and then read again the rest of the settings. */
        WnckScreen *screen = wnck_screen_get_default();
        int token = wnck_screen_try_set_workspace_layout(screen, 0, rows, 0);
        if (token != 0) {
            wnck_screen_release_workspace_layout(screen, token);
            return true;
        } else {
            qWarning() << "Failed to set workspaces layout via WNCK.";
        }
    } else {
       qWarning("Failed to read number of workspace rows from configuration.");
    }
    return false;
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
        /* In this property does not exist (as is the case if you
           don't login into the regular gnome session before unity-2d),
           read the values available in gconf and calculate the others,
           then set layout via WNCK and read the property again. */
        bool success = setWorkspacesGeometryFromConfiguration();
        if (success) {
            result = getX11IntProperty(_NET_DESKTOP_LAYOUT, 4);
        }
    }
    if (result != NULL) {
        /* If we read the values correctly the first time or re-read them
           correctly after setting them from config, just use them. */
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
    } else {
        /* If we are still failing, then just set some decent defaults. */
        rows = 1;
        columns = workspaces;
        orientation = ScreenInfo::OrientationHorizontal;
        startingCorner = ScreenInfo::CornerTopLeft;
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

QString ScreenInfo::desktopFileForApplication(int applicationId)
{
    BamfApplication* application = BamfMatcher::get_default().application_for_xid(applicationId);
    return (application != NULL) ? application->desktop_file() : QString();
}

void ScreenInfo::onActiveWindowChanged(WnckScreen *screen,
                                       WnckWindow *previously_active_window,
                                       gpointer    user_data)
{
    Q_UNUSED(previously_active_window);
    Q_UNUSED(user_data);

    ScreenInfo::instance()->updateActiveWindow(screen);
}

void ScreenInfo::updateActiveWindow(WnckScreen *screen)
{
    unsigned int activeWindow = 0;
    WnckWindow *wnckActiveWindow = wnck_screen_get_active_window(screen);
    if (wnckActiveWindow != NULL) {
        activeWindow = wnck_window_get_xid(wnckActiveWindow);
    }

    if (activeWindow != m_activeWindow) {
        m_activeWindow = activeWindow;
        Q_EMIT activeWindowChanged(m_activeWindow);
    }
}

/* FIXME: this will not work with wm using large desktops and viewports
   to implement their workspaces.*/
void ScreenInfo::activateWorkspace(int workspaceNumber)
{
    WnckScreen *screen = wnck_screen_get_default();
    WnckWorkspace *workspace = wnck_screen_get_workspace(screen, workspaceNumber);
    if (workspace == NULL) {
        qWarning() << "Requested activation workspace" << workspaceNumber << " but it does not exist.";
        return;
    }

    wnck_workspace_activate(workspace, CurrentTime);
}

/* FIXME: This should be removed when we find a cleaner way to bypass the
   QML Image cache. See SpreadWindow.qml and WindowImageProvider::requestImage
   for details. */
QString ScreenInfo::currentTime()
{
    return QString::number(time(NULL));
}

QRect ScreenInfo::availableGeometry() const
{
    return QApplication::desktop()->availableGeometry(QX11Info::appScreen());
}

QRect ScreenInfo::geometry() const
{
    return QApplication::desktop()->screenGeometry(QX11Info::appScreen());
}

void ScreenInfo::updateGeometry(int screen)
{
    qDebug() << "geometry updated on screen" << screen;
    if (screen == QX11Info::appScreen()) {
        Q_EMIT geometryChanged(geometry());
    }
}

void ScreenInfo::updateAvailableGeometry(int screen)
{
    qDebug() << "available geometry updated on screen" << screen;
    if (screen == QX11Info::appScreen()) {
        Q_EMIT availableGeometryChanged(availableGeometry());
    }
}

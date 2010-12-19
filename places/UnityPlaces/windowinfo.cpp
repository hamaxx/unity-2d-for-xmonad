#include <libwnck/screen.h>
#include <libwnck/window.h>
#include <libwnck/workspace.h>

#include <glib-2.0/glib.h>

#include "bamf-matcher.h"
#include "bamf-application.h"
#include "bamf-window.h"

#include "windowinfo.h"
#include <X11/X.h>


WindowInfo::WindowInfo(unsigned int xid, QObject *parent) :
    QObject(parent), m_bamfApplication(NULL), m_bamfWindow(NULL), m_wnckWindow(NULL), m_xid(0)
{
    setXid(xid);
}

unsigned int WindowInfo::xid() const {
    return m_xid;
}

void WindowInfo::setXid(unsigned int xid) {
    if (xid == m_xid) {
        return;
    }

    /* Find out BamfApplication and BamfWindow correponding to xid */
    BamfApplication *bamfApplication = BamfMatcher::get_default().application_for_xid(xid);
    if (bamfApplication == NULL) {
        return;
    }

    BamfWindow *bamfWindow;
    BamfWindowList *bamfWindows = bamfApplication->windows();
    for (int i = 0; i < bamfWindows->size(); i++) {
        bamfWindow = bamfWindows->at(i);
        if (bamfWindow->xid() == xid) {
            break;
        }
    }

    if (bamfWindow == NULL) {
        return;
    }

    /* Find out WnckWindow and BamfWindow correponding to xid */
    WnckWindow *wnckWindow = wnck_window_get(xid);
    if (wnckWindow == NULL) {
        wnck_screen_force_update(wnck_screen_get_default());
        wnckWindow = wnck_window_get(xid);
    }

    if (wnckWindow == NULL) {
        return;
    }

    /* Set member variables and emit changed signals */
    m_bamfApplication = bamfApplication;
    m_bamfWindow = bamfWindow;
    m_wnckWindow = wnckWindow;
    m_xid = xid;

    emit xidChanged(xid);

    updateGeometry();

    emit zChanged(z());
    emit titleChanged(title());
    emit iconChanged(icon());
}

QPoint WindowInfo::position() const {
    return m_position;
}

QSize WindowInfo::size() const {
    return m_size;
}

unsigned int WindowInfo::z() const {
    int z = 0;
    GList *stack = wnck_screen_get_windows_stacked(wnck_screen_get_default());
    GList *cur = stack;
    while (cur) {
        z++;
        WnckWindow *window = (WnckWindow*) cur->data;
        if (wnck_window_get_xid(window) == m_xid) {
            break;
        }
        cur = g_list_next(cur);
    }
    return z;
}

QString WindowInfo::title() const {
    return (m_bamfWindow == NULL) ? QString() : m_bamfWindow->name();
}

QString WindowInfo::icon() const {
    /* m_bamfWindow and m_bamfApplication should always both
       be null or non-null at the same time. */
    if (m_bamfWindow == NULL) {
        return QString();
    }
    return (m_bamfWindow->icon().isEmpty()) ?
            m_bamfApplication->icon() : m_bamfWindow->icon();

}

void WindowInfo::activate() {
    showWindow(m_wnckWindow);
}

void WindowInfo::updateGeometry() {
    int x, y, w, h;

    wnck_window_get_client_window_geometry(m_wnckWindow, &x, &y, &w, &h);

    m_position.setX(x);
    m_position.setY(y);
    m_size.setWidth(w);
    m_size.setHeight(h);

    emit positionChanged(m_position);
    emit sizeChanged(m_size);
}

/* FIXME: copied from UnityApplications/launcherapplication.cpp */
void WindowInfo::showWindow(WnckWindow* window)
{
    WnckWorkspace* workspace = wnck_window_get_workspace(window);

    /* Using X.h's CurrentTime (= 0) */
    wnck_workspace_activate(workspace, CurrentTime);

    /* If the workspace contains a viewport then move the viewport so
       that the window is visible.
       Compiz for example uses only one workspace with a desktop larger
       than the screen size which means that a viewport is used to
       determine what part of the desktop is visible.

       Reference:
       http://standards.freedesktop.org/wm-spec/wm-spec-latest.html#largedesks
    */
    if (wnck_workspace_is_virtual(workspace)) {
        moveViewportToWindow(window);
    }

    /* Using X.h's CurrentTime (= 0) */
    wnck_window_activate(window, CurrentTime);
}

/* FIXME: copied from UnityApplications/launcherapplication.cpp */
void WindowInfo::moveViewportToWindow(WnckWindow* window) {
    WnckWorkspace* workspace = wnck_window_get_workspace(window);
    WnckScreen* screen = wnck_window_get_screen(window);

    int screen_width = wnck_screen_get_width(screen);
    int screen_height = wnck_screen_get_height(screen);
    int viewport_x = wnck_workspace_get_viewport_x(workspace);
    int viewport_y = wnck_workspace_get_viewport_y(workspace);

    int window_x, window_y, window_width, window_height;
    wnck_window_get_geometry(window, &window_x, &window_y,
                                     &window_width, &window_height);

    /* Compute the row and column of the "virtual workspace" that contains
       the window. A "virtual workspace" is a portion of the desktop of the
       size of the screen.
    */
    int viewport_column = (viewport_x + window_x) / screen_width;
    int viewport_row = (viewport_y + window_y) / screen_height;

    wnck_screen_move_viewport(screen, viewport_column * screen_width,
                                      viewport_row * screen_height);
}


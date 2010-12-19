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
    QObject(parent), m_bamfWindow(0)
{
    if (xid == 0) return;
    fromXid(xid);
}

void WindowInfo::fromXid(unsigned int xid) {
   m_bamfApplication = BamfMatcher::get_default().application_for_xid(xid);
   if (m_bamfApplication == 0) {
       m_bamfWindow = 0;
       return;
   }

   BamfWindowList *windows = m_bamfApplication->windows();
   for (int i = 0; i < windows->size(); i++) {
       BamfWindow *window = windows->at(i);
       if (window->xid() == xid) {
           m_bamfWindow = window;
           break;
       }
   }

   if (!m_bamfWindow) {
       m_bamfApplication = 0;
       return;
   }

   connect(m_bamfWindow, SIGNAL(ActiveChanged(bool)), SLOT(onActiveChanged(bool)));
}

void WindowInfo::onActiveChanged(bool active) {
    emit activeChanged(active);
}

unsigned int WindowInfo::xid() const {
    return m_bamfWindow->xid();
}

void WindowInfo::setXid(unsigned int varXid) {
    if (xid() == varXid) {
        return;
    }

    fromXid(varXid);
    emit xidChanged(xid());
    emit windowChanged(m_bamfWindow);

    QSize size;
    QPoint location;
    int z;
    geometry(m_bamfWindow->xid(), &size, &location, &z);
    emit sizeChanged(size);
    emit positionChanged(location);
    emit zChanged(z);

    emit applicationNameChanged(applicationName());
    emit titleChanged(title());
    emit iconChanged(icon());
}

QPoint WindowInfo::position() const {
    QPoint location;
    geometry(m_bamfWindow->xid(), 0, &location, 0);
    return location;
}

QSize WindowInfo::size() const {
    QSize size;
    geometry(m_bamfWindow->xid(), &size, 0, 0);
    return size;
}

int WindowInfo::z() const {
    int z;
    geometry(m_bamfWindow->xid(), 0, 0, &z);
    return z;
}

QString WindowInfo::applicationName() const {
    return (m_bamfApplication == 0) ? QString() : m_bamfApplication->name();
}

QString WindowInfo::title() const {
    return (m_bamfWindow == 0) ? QString() : m_bamfWindow->name();
}

QString WindowInfo::icon() const {
    /* m_bamfWindow and m_bamfApplication should always both
       be null or non-null at the same time. */
    if (m_bamfWindow == 0) {
        return QString();
    }
    return (m_bamfWindow->icon().isEmpty()) ?
            m_bamfApplication->icon() : m_bamfWindow->icon();

}

bool WindowInfo::active() const {
    return (m_bamfWindow) ? m_bamfWindow->active() : false;
}

void WindowInfo::activate() {
    WnckWindow *window = getWnckWindow();
    if (window == 0) {
        return;
    }

    showWindow(window);
}

WnckWindow* WindowInfo::getWnckWindow(unsigned int xid) const {
    if (xid == 0) {
        if (m_bamfWindow == 0) {
            return 0;
        }
        xid = m_bamfWindow->xid();
    }
    WnckWindow *window = wnck_window_get(xid);
    if (window == 0) {
        wnck_screen_force_update(wnck_screen_get_default());
        window = wnck_window_get(xid);
    }
    return window;
}

bool WindowInfo::geometry(unsigned int xid, QSize *size, QPoint *position, int *z) const {
    int x, y, w, h;

    if (!xid) {
        return false;
    }

    WnckWindow *window = getWnckWindow(xid);
    if (window == 0) {
        return false;
    }

    wnck_window_get_client_window_geometry(window, &x, &y, &w, &h);

    if (size) {
        size->setWidth(w);
        size->setHeight(h);
    }
    if (position) {
        position->setX(x);
        position->setY(y);
    }

    if (z) {
        *z = 0;
        unsigned int i = 0;
        GList *stack = wnck_screen_get_windows_stacked(wnck_screen_get_default());
        GList *cur = stack;
        while (cur) {
            i++;
            WnckWindow *window = (WnckWindow*) cur->data;
            if (wnck_window_get_xid(window) == xid) {
                *z = i;
                break;
            }
            cur = g_list_next(cur);
        }
     }

    return true;
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


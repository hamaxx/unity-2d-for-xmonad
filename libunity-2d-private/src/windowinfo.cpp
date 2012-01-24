/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <libwnck/libwnck.h>

#include <glib-2.0/glib.h>

#include "bamf-matcher.h"
#include "bamf-application.h"
#include "bamf-window.h"

#include "windowinfo.h"
#include <X11/Xlib.h>
#include <QX11Info>

WindowInfo::WindowInfo(unsigned int contentXid, QObject *parent) :
    QObject(parent), m_bamfApplication(NULL), m_bamfWindow(NULL), m_wnckWindow(NULL),
    m_contentXid(0), m_decoratedXid(0)
{
    setContentXid(contentXid);
}

WindowInfo::~WindowInfo()
{
    g_signal_handlers_disconnect_by_func(m_wnckWindow, gpointer(WindowInfo::onWorkspaceChanged), this);
}

unsigned int WindowInfo::contentXid() const
{
    return m_contentXid;
}

BamfWindow* WindowInfo::getBamfWindowForApplication(BamfApplication *application,
                                                    unsigned int xid)
{
    if (application == NULL) {
        return NULL;
    }

    BamfWindow *window = NULL;
    BamfWindowList *windows = application->windows();
    for (int i = 0; i < windows->size(); i++) {
        window = windows->at(i);
        if (window->xid() == xid) {
            break;
        }
    }

    return window;
}

WnckWindow* WindowInfo::getWnckWindowForXid(unsigned int xid)
{
    /* Since WNCK does not update its window list in real time,
       it may be possible that it doesn't know yet know about xid.
       In that case, we first ask it to update its list, then try
       again. */
    WnckWindow *window = wnck_window_get(xid);
    if (window == NULL) {
        wnck_screen_force_update(wnck_screen_get_default());
        window = wnck_window_get(xid);
    }
    return window;
}

unsigned int WindowInfo::findTopmostAncestor(unsigned int xid)
{
    unsigned long root, *children;
    unsigned long parent = xid;
    unsigned int nchildren;
    unsigned int topmost;
    do {
        topmost = parent;

        if (XQueryTree (QX11Info::display(), topmost, &root,
                        &parent, &children, &nchildren) == 0) {
            /* In case the query fails, fallback to our original xid */
            topmost = xid;
            break;
        }
    } while (parent != root);

    return topmost;
}

void WindowInfo::setContentXid(unsigned int contentXid)
{
    if (contentXid == m_contentXid) {
        return;
    }

    /* First figure out what's the BamfApplication to which the content Xid
       belongs. However what we need is the actual BamfWindow, so we search
       for it among all the BamfWindows for that app. */
    BamfApplication *bamfApplication;
    bamfApplication = BamfMatcher::get_default().application_for_xid(contentXid);
    BamfWindow *bamfWindow = getBamfWindowForApplication(bamfApplication, contentXid);
    if (bamfWindow == NULL) {
        return;
    }

    /* We ask WNCK to give us its own WnckWindow, since BAMF doesn't
       provide us enough features to handle on its own all we need. */
    WnckWindow *wnckWindow = getWnckWindowForXid(contentXid);
    if (wnckWindow == NULL) {
        return;
    }

    /* The above would be enough if we wanted the spread to only work by
       displaying screenshots of the window contents.
       However we want also the decorations, to maximize the illusion that we
       are actually spreading the real windows.
       Therefore we climb up the X11 window tree to find the topmost ancestor
       for the content window, which should be where the WM places the decorations.
    */
    unsigned int decoratedXid = findTopmostAncestor(contentXid);

    /* Disconnect previously connected signals */
    if (m_wnckWindow != NULL) {
        g_signal_handlers_disconnect_by_func(m_wnckWindow, gpointer(WindowInfo::onWorkspaceChanged), this);
    }

    /* Set member variables and emit changed signals */
    m_bamfApplication = bamfApplication;
    m_bamfWindow = bamfWindow;
    m_wnckWindow = wnckWindow;
    m_contentXid = contentXid;
    m_decoratedXid = decoratedXid;

    g_signal_connect(G_OBJECT(m_wnckWindow), "workspace-changed",
                     G_CALLBACK(WindowInfo::onWorkspaceChanged), this);

    Q_EMIT contentXidChanged(m_contentXid);
    Q_EMIT decoratedXidChanged(m_decoratedXid);

    updateGeometry();

    Q_EMIT zChanged(z());
    Q_EMIT titleChanged(title());
    Q_EMIT iconChanged(icon());
    Q_EMIT desktopFileChanged(desktopFile());
    Q_EMIT workspaceChanged(workspace());
}

void WindowInfo::setWorkspace(int workspaceNumber)
{
    WnckWorkspace* workspace = wnck_screen_get_workspace(wnck_screen_get_default(), workspaceNumber);
    wnck_window_move_to_workspace(m_wnckWindow, workspace);
}

unsigned int WindowInfo::decoratedXid() const
{
    return m_decoratedXid;
}

QPoint WindowInfo::position() const
{
    return m_position;
}

QSize WindowInfo::size() const
{
    return m_size;
}

unsigned int WindowInfo::z() const
{
    int z = 0;
    GList *stack = wnck_screen_get_windows_stacked(wnck_screen_get_default());
    GList *cur = stack;
    while (cur) {
        z++;
        WnckWindow *window = (WnckWindow*) cur->data;
        if (wnck_window_get_xid(window) == m_contentXid) {
            break;
        }
        cur = g_list_next(cur);
    }
    return z;
}

QString WindowInfo::title() const
{
    return (m_bamfWindow == NULL) ? QString() : m_bamfWindow->name();
}

QString WindowInfo::icon() const
{
    /* m_bamfWindow and m_bamfApplication will always both
       be null or non-null at the same time, so we can just check one. */
    if (m_bamfWindow == NULL) {
        return QString();
    }
    return (m_bamfWindow->icon().isEmpty()) ?
            m_bamfApplication->icon() : m_bamfWindow->icon();

}

QString WindowInfo::desktopFile() const
{
    if (m_bamfApplication == NULL) {
        return QString();
    }
    return m_bamfApplication->desktop_file();
}

/* This returns the number of the workspace where the window is.
   It return -1 if the window is not on any workspace (not sure if this can happen,
   but better be safe rather than sorry).
   And as a special case it does return -2 for a window that is on all workspaces
   (also called a "pinned" window).
*/
int WindowInfo::workspace() const
{
    if (m_wnckWindow != NULL) {
        WnckWorkspace *workspace = wnck_window_get_workspace(m_wnckWindow);
        if (workspace != NULL) {
            return wnck_workspace_get_number(workspace);
        } else {
            if (wnck_window_is_pinned(m_wnckWindow)) {
                return -2;
            }
        }
    }

    return -1;
}

void WindowInfo::activate()
{
    showWindow(m_wnckWindow);
}

void WindowInfo::updateGeometry()
{
    int x, y, w, h;

    wnck_window_get_geometry(m_wnckWindow, &x, &y, &w, &h);

    m_position.setX(x);
    m_position.setY(y);
    m_size.setWidth(w);
    m_size.setHeight(h);

    Q_EMIT positionChanged(m_position);
    Q_EMIT sizeChanged(m_size);
}

void WindowInfo::showWindow(WnckWindow* window)
{
    wnck_window_activate(window, CurrentTime);
}

bool WindowInfo::isSameBamfWindow(BamfWindow *other)
{
    return (m_bamfWindow == other);
}

void WindowInfo::onWorkspaceChanged(WnckWindow *window, gpointer user_data)
{
    Q_UNUSED(window);

    WindowInfo *instance = static_cast<WindowInfo*>(user_data);
    if (instance != NULL) {
        instance->updateWorkspace();
    }
}

void WindowInfo::updateWorkspace()
{
    Q_EMIT workspaceChanged(workspace());
}

#include "windowinfo.moc"

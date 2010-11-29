#include <libwnck/screen.h>
#include <libwnck/window.h>
#include <glib-2.0/glib.h>

#include <QDebug>

#include "windowinfo.h"

#include "bamf-matcher.h"
#include "bamf-application.h"
#include "bamf-window.h"

WindowInfo::WindowInfo(Window xid, QObject *parent) :
    QObject(parent), m_bamfWindow(0)
{
    if (xid == 0) return;
    fromXid(xid);
}

void WindowInfo::fromXid(Window xid) {
   m_bamfApplication = BamfMatcher::get_default().application_for_xid(xid);
   if (m_bamfApplication == 0) {
       m_bamfWindow = 0;
       return;
   }

   BamfWindowList *wins = m_bamfApplication->windows();
   for (int i = 0; i < wins->size(); i++) {
       BamfWindow *win = wins->at(i);
       if (win->xid() == xid) {
           m_bamfWindow = win;
           return;
       }
   }
}

QVariant WindowInfo::xid() const {
    return QVariant::fromValue(m_bamfWindow->xid());
}

void WindowInfo::setXid(QVariant varXid) {
    if (xid() == varXid) {
        return;
    }

    fromXid(varXid.toULongLong());
    emit xidChanged(xid());
    emit windowChanged(m_bamfWindow);

    QSize size;
    QPoint location;
    int z;
    geometry(m_bamfWindow->xid(), &size, &location, &z);
    emit sizeChanged(size);
    emit locationChanged(location);
    emit zChanged(z);

    emit appNameChanged(appName());
    emit titleChanged(title());
    emit iconChanged(icon());
}

QPoint WindowInfo::location() const {
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

QString WindowInfo::appName() const {
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

bool WindowInfo::geometry(Window xid, QSize *size, QPoint *position, int *z) const {
    int x, y, w, h;

    if (!xid) {
        return false;
    }

    WnckWindow *win = wnck_window_get(xid);
    if (win == 0) {
        qDebug() << "!!!!!!!!!!! Had to force update";
        wnck_screen_force_update(wnck_screen_get_default());
        win = wnck_window_get(xid);
    }
    if (win == 0) return false;

    wnck_window_get_client_window_geometry(win, &x, &y, &w, &h);

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
            WnckWindow *win = (WnckWindow*) cur->data;
            if (wnck_window_get_xid(win) == xid) {
                *z = i;
                break;
            }
            cur = g_list_next(cur);
        }
     }

    return true;
}

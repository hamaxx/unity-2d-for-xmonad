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

// Local
#include <config.h>
#include "shelldeclarativeview.h"
#include "dashdbus.h"

// libunity-2d-private
#include <debug_p.h>
#include <hotkey.h>
#include <screeninfo.h>
#include <strutmanager.h>

// Qt
#include <QApplication>
#include <QBitmap>
#include <QCloseEvent>
#include <QDeclarativeContext>
#include <QDesktopWidget>
#include <QtDBus/QDBusConnection>
#include <QtDBus/QDBusConnectionInterface>
#include <QtDBus/QDBusInterface>
#include <QX11Info>
#include <QGraphicsObject>
#include <QFileInfo>

// X11
#include <X11/Xlib.h>
#include <X11/Xatom.h>

ShellDeclarativeView::ShellDeclarativeView(const QUrl &sourceFileUrl, bool isTopLeftShell, int screen)
    : Unity2DDeclarativeView()
    , m_isTopLeftShell(isTopLeftShell)
    , m_sourceFileUrl(sourceFileUrl)
{
    setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);
    setTransparentBackground(QX11Info::isCompositingManagerRunning());

    m_screenInfo = new ScreenInfo(screen, this);

    connect(m_screenInfo, SIGNAL(availableGeometryChanged(QRect)), SLOT(updateShellPosition()));
    updateShellPosition();
}

void
ShellDeclarativeView::updateShellPosition()
{
    // ShellDeclarativeView is a dock window (Qt::WA_X11NetWmWindowTypeDock) this means it does not respect struts.
    // We use availableGeometry to get the geometry with the struts applied and from there
    // we remove any strut that we might be applying ourselves
    const QRect availableGeometry = m_screenInfo->availableGeometry();
    QPoint posToMove = availableGeometry.topLeft();
    if (qApp->isRightToLeft()) {
        posToMove.setX(availableGeometry.width() - width());
    }

    if (rootObject() != NULL) {
        QList<StrutManager *> strutManagers = rootObject()->findChildren<StrutManager*>();
        Q_FOREACH(StrutManager *strutManager, strutManagers) {
            if (strutManager->enabled()) {
                // Do not push ourselves
                switch (strutManager->edge()) {
                    case Unity2dPanel::TopEdge:
                        posToMove.ry() -= strutManager->realHeight();
                    break;

                    case Unity2dPanel::LeftEdge:
                        if (qApp->isLeftToRight()) {
                            posToMove.rx() -= strutManager->realWidth();
                        } else {
                            posToMove.rx() += strutManager->realWidth();
                        }
                    break;
                }
            }
        }
    }

    move(posToMove);
}

void
ShellDeclarativeView::focusOutEvent(QFocusEvent* event)
{
    Unity2DDeclarativeView::focusOutEvent(event);
// TODO    setDashActive(false);
    Q_EMIT focusChanged();
}

void
ShellDeclarativeView::focusInEvent(QFocusEvent* event)
{
    Unity2DDeclarativeView::focusInEvent(event);
    Q_EMIT focusChanged();
}

void
ShellDeclarativeView::resizeEvent(QResizeEvent *event)
{
    updateShellPosition();
    Unity2DDeclarativeView::resizeEvent(event);
}

void
ShellDeclarativeView::setWMFlags()
{
    Display *display = QX11Info::display();
    Atom stateAtom = XInternAtom(display, "_NET_WM_STATE", False);
    Atom propAtom;

    propAtom = XInternAtom(display, "_NET_WM_STATE_SKIP_TASKBAR", False);
    XChangeProperty(display, effectiveWinId(), stateAtom,
                    XA_ATOM, 32, PropModeAppend, (unsigned char *) &propAtom, 1);

    propAtom = XInternAtom(display, "_NET_WM_STATE_SKIP_PAGER", False);
    XChangeProperty(display, effectiveWinId(), stateAtom,
                    XA_ATOM, 32, PropModeAppend, (unsigned char *) &propAtom, 1);
}

void
ShellDeclarativeView::showEvent(QShowEvent *event)
{
    Unity2DDeclarativeView::showEvent(event);
    /* Note that this has to be called everytime the window is shown, as the WM
       will remove the flags when the window is hidden */
    setWMFlags();
    if (source().isEmpty()) {
        QMap<const char*, QVariant> rootObjectProperties;
        rootObjectProperties.insert("declarativeView", QVariant::fromValue(this));
        setSource(m_sourceFileUrl, rootObjectProperties);
    }
}

void
ShellDeclarativeView::toggleLauncher()
{
    if (!isActiveWindow()) {
        forceActivateWindow();
        Q_EMIT launcherFocusRequested();
    } else {
        forceDeactivateWindow();
    }
}

void
ShellDeclarativeView::processNumericHotkey(Hotkey* hotkey)
{
    /* Shortcuts from 1 to 9 should activate the items with index
       from 1 to 9 (index 0 being the so-called "BFB" or Dash launcher).
       Shortcut for 0 should activate item with index 10.
       In other words, the indexes are activated in the same order as
       the keys appear on a standard keyboard. */
    Qt::Key key = hotkey->key();
    if (key >= Qt::Key_1 && key <= Qt::Key_9) {
        int index = key - Qt::Key_0;
        if (hotkey->modifiers() & Qt::ShiftModifier) {
            Q_EMIT newInstanceShortcutPressed(index);
        } else {
            Q_EMIT activateShortcutPressed(index);
        }
    } else if (key == Qt::Key_0) {
        if (hotkey->modifiers() & Qt::ShiftModifier) {
            Q_EMIT newInstanceShortcutPressed(10);
        } else {
            Q_EMIT activateShortcutPressed(10);
        }
    }
}

/* ----------------- monitored area handling ---------------- */

void
ShellDeclarativeView::mouseMoveEvent(QMouseEvent *event)
{
    Unity2DDeclarativeView::mouseMoveEvent(event);

    bool containsMouse = m_monitoredArea.contains(event->pos());
    if (m_monitoredAreaContainsMouse != containsMouse) {
        m_monitoredAreaContainsMouse = containsMouse;
        Q_EMIT monitoredAreaContainsMouseChanged();
    }
}

void
ShellDeclarativeView::leaveEvent(QEvent *event)
{
    Unity2DDeclarativeView::leaveEvent(event);

    if (m_monitoredAreaContainsMouse) {
        m_monitoredAreaContainsMouse = false;
        Q_EMIT monitoredAreaContainsMouseChanged();
    }
}

/* When another window calls XGrabPointer we receive a LeaveNotify event
   but QT doesn't emit a corresponding leaveEvent. Therefore we have to intercept it
   ourselves from X11 and act accordingly.
   The same is true for the opposite, when XUngrabPointer is called. */
bool
ShellDeclarativeView::x11EventFilter(XEvent* event)
{
    if (event->type == LeaveNotify && event->xcrossing.mode == NotifyGrab) {
        if (m_monitoredAreaContainsMouse) {
            m_monitoredAreaContainsMouse = false;
            Q_EMIT monitoredAreaContainsMouseChanged();
        }
    } else if (event->type == EnterNotify && event->xcrossing.mode == NotifyUngrab) {
        if (!m_monitoredAreaContainsMouse) {
            m_monitoredAreaContainsMouse = true;
            Q_EMIT monitoredAreaContainsMouseChanged();
        }
    }
    return false;
}

QRect
ShellDeclarativeView::monitoredArea() const
{
    return m_monitoredArea;
}

void
ShellDeclarativeView::setMonitoredArea(QRect monitoredArea)
{
    if (m_monitoredArea != monitoredArea) {
        m_monitoredArea = monitoredArea;
        Q_EMIT monitoredAreaChanged();

        bool containsMouse = monitoredArea.contains(mapFromGlobal(QCursor::pos()));
        if (containsMouse != m_monitoredAreaContainsMouse) {
            m_monitoredAreaContainsMouse = containsMouse;
            Q_EMIT monitoredAreaContainsMouseChanged();
        }
    }
}

bool
ShellDeclarativeView::monitoredAreaContainsMouse() const
{
    return m_monitoredAreaContainsMouse;
}

void
ShellDeclarativeView::setIsTopLeftShell(bool ashell)
{
    if (m_isTopLeftShell == ashell) {
        return;
    }

    m_isTopLeftShell = ashell;
    Q_EMIT isTopLeftShellChanged(m_isTopLeftShell);
}

void
ShellDeclarativeView::setScreenNumber(int screen)
{
    m_screenInfo->setScreen(screen);
}

int
ShellDeclarativeView::screenNumber() const
{
    return m_screenInfo->screen();
}

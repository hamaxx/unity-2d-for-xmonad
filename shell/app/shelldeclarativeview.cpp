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
#include "shellmanager.h"
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
#include <QDeclarativeItem>
#include <QFileInfo>

// X11
#include <X11/Xlib.h>
#include <X11/Xatom.h>

ShellDeclarativeView::ShellDeclarativeView(ShellManager *manager, const QUrl &sourceFileUrl, int screen)
    : Unity2DDeclarativeView()
    , m_monitoredAreaContainsMouse(false)
    , m_sourceFileUrl(sourceFileUrl)
    , m_manager(manager)
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
        posToMove.rx() += (availableGeometry.width() - width());
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
    if (rootObject()) {
        const int wantedWidth = rootObject()->property("width").toInt();
        const int wantedHeight = rootObject()->property("height").toInt();
        if (width() != wantedWidth || height() != wantedHeight) {
            // FIXME This should never happen as we are using SizeViewToRootObject
            // in our QDeclarativeView but it seems it happens sometimes
            // that we get a size it's not the size the QML wants
            // so force it to the size we want in those cases
            // This is just a workaround for the time being
            resize(wantedWidth, wantedHeight);
        }
    }
    updateShellPosition();
    Unity2DDeclarativeView::resizeEvent(event);
}


void ShellDeclarativeView::forceActivateWindow()
{
    m_manager->forceActivateShell(this);
}

void ShellDeclarativeView::forceDeactivateWindow()
{
    m_manager->forceDeactivateShell(this);
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

void ShellDeclarativeView::dragLeaveEvent(QDragLeaveEvent *event)
{
    Unity2DDeclarativeView::dragLeaveEvent(event);

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

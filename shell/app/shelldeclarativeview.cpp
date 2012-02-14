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
#include <hotkeymonitor.h>
#include <keyboardmodifiersmonitor.h>
#include <keymonitor.h>
#include <dashclient.h>
#include <launcherclient.h>
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

static const int KEY_HOLD_THRESHOLD = 250;

static const char* COMMANDS_LENS_ID = "commands.lens";

ShellDeclarativeView::ShellDeclarativeView(const QUrl &sourceFileUrl, bool isTopLeftShell, int screen)
    : Unity2DDeclarativeView()
    , m_mode(DesktopMode)
    , m_expanded(true)
    , m_active(false)
    , m_superKeyPressed(false)
    , m_superKeyHeld(false)
    , m_isTopLeftShell(isTopLeftShell)
    , m_sourceFileUrl(sourceFileUrl)
{
    setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);
    setTransparentBackground(QX11Info::isCompositingManagerRunning());

    m_screenInfo = new ScreenInfo(screen, this);

    m_superKeyHoldTimer.setSingleShot(true);
    m_superKeyHoldTimer.setInterval(KEY_HOLD_THRESHOLD);
    connect(&m_superKeyHoldTimer, SIGNAL(timeout()), SLOT(updateSuperKeyHoldState()));
    connect(this, SIGNAL(superKeyTapped()), SLOT(toggleDash()));

    connect(&launcher2dConfiguration(), SIGNAL(superKeyEnableChanged(bool)), SLOT(updateSuperKeyMonitoring()));
    updateSuperKeyMonitoring();

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

    move(posToMove);
}

void
ShellDeclarativeView::focusOutEvent(QFocusEvent* event)
{
    Unity2DDeclarativeView::focusOutEvent(event);
    setDashActive(false);
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
        setSource(m_sourceFileUrl);
    }
}

void
ShellDeclarativeView::setDashActive(bool value)
{
    if (value != m_active) {
        m_active = value;
        Q_EMIT dashActiveChanged(m_active);
    }
}

bool
ShellDeclarativeView::dashActive() const
{
    return m_active;
}

bool
ShellDeclarativeView::haveCustomHomeShortcuts() const
{
    return QFileInfo(unity2dDirectory() + "/shell/dash/HomeShortcutsCustomized.qml").exists();
}

void
ShellDeclarativeView::setDashMode(ShellDeclarativeView::DashMode mode)
{
    if (m_mode == mode) {
        return;
    }

    m_mode = mode;
    dashModeChanged(m_mode);
}

ShellDeclarativeView::DashMode
ShellDeclarativeView::dashMode() const
{
    return m_mode;
}

void
ShellDeclarativeView::setExpanded(bool value)
{
    if (m_expanded == value) {
        return;
    }

    m_expanded = value;
    expandedChanged(m_expanded);
}

bool
ShellDeclarativeView::expanded() const
{
    return m_expanded;
}

void
ShellDeclarativeView::setActiveLens(const QString& activeLens)
{
    if (activeLens != m_activeLens) {
        m_activeLens = activeLens;
        Q_EMIT activeLensChanged(activeLens);
    }
}

const QString&
ShellDeclarativeView::activeLens() const
{
    return m_activeLens;
}

void
ShellDeclarativeView::toggleDash()
{
    if (dashActive()) {
        setDashActive(false);
        forceDeactivateWindow();
    } else {
        Q_EMIT activateHome();
    }
}

void
ShellDeclarativeView::showCommandsLens()
{
    Q_EMIT activateLens(COMMANDS_LENS_ID);
}

void
ShellDeclarativeView::toggleLauncher()
{
    if (!isActiveWindow()) {
        forceActivateWindow();
        Q_EMIT launcherFocusRequested();
    } else {
        if (dashActive()) {
            // focus the launcher instead of the dash
            setDashActive(false);
            Q_EMIT launcherFocusRequested();
        } else {
            // we assume that the launcher is focused; unfocus it by deactivating the shell window
            forceDeactivateWindow();
        }
    }
}

/* ----------------- super key handling ---------------- */

void
ShellDeclarativeView::updateSuperKeyHoldState()
{
    /* If the key was released in the meantime, just do nothing, otherwise
       consider the key being held, unless we're told to ignore it. */
    if (m_superKeyPressed && !m_superPressIgnored) {
        m_superKeyHeld = true;
        Q_EMIT superKeyHeldChanged(m_superKeyHeld);
    }
}

void
ShellDeclarativeView::updateSuperKeyMonitoring()
{
    KeyboardModifiersMonitor *modifiersMonitor = KeyboardModifiersMonitor::instance();
    KeyMonitor *keyMonitor = KeyMonitor::instance();
    HotkeyMonitor& hotkeyMonitor = HotkeyMonitor::instance();

    QVariant value = launcher2dConfiguration().property("superKeyEnable");
    if (!value.isValid() || value.toBool() == true) {
        hotkeyMonitor.enableModifiers(Qt::MetaModifier);
        QObject::connect(modifiersMonitor,
                         SIGNAL(keyboardModifiersChanged(Qt::KeyboardModifiers)),
                         this, SLOT(setHotkeysForModifiers(Qt::KeyboardModifiers)));
        /* Ignore Super presses if another key was pressed simultaneously
           (i.e. a shortcut). https://bugs.launchpad.net/unity-2d/+bug/801073 */
        QObject::connect(keyMonitor,
                         SIGNAL(keyPressed()),
                         this, SLOT(ignoreSuperPress()));
        setHotkeysForModifiers(modifiersMonitor->keyboardModifiers());
    } else {
        hotkeyMonitor.disableModifiers(Qt::MetaModifier);
        QObject::disconnect(modifiersMonitor,
                            SIGNAL(keyboardModifiersChanged(Qt::KeyboardModifiers)),
                            this, SLOT(setHotkeysForModifiers(Qt::KeyboardModifiers)));
        QObject::disconnect(keyMonitor,
                            SIGNAL(keyPressed()),
                            this, SLOT(ignoreSuperPress()));
        m_superKeyHoldTimer.stop();
        m_superKeyPressed = false;
        if (m_superKeyHeld) {
            m_superKeyHeld = false;
            Q_EMIT superKeyHeldChanged(false);
        }
    }
}

void
ShellDeclarativeView::setHotkeysForModifiers(Qt::KeyboardModifiers modifiers)
{
    /* This is the new new state of the Super key (AKA Meta key), while
       m_superKeyPressed is the previous state of the key at the last modifiers change. */
    bool superKeyPressed = modifiers.testFlag(Qt::MetaModifier);

    if (m_superKeyPressed != superKeyPressed) {
        m_superKeyPressed = superKeyPressed;
        if (superKeyPressed) {
            m_superPressIgnored = false;
            /* If the key is pressed, start up a timer to monitor if it's being held short
               enough to qualify as just a "tap" or as a proper hold */
            m_superKeyHoldTimer.start();
        } else {
            m_superKeyHoldTimer.stop();

            /* If the key is released, and was not being held, it means that the user just
               performed a "tap". Unless we're told to ignore that tap, that is. */
            if (!m_superKeyHeld && !m_superPressIgnored) {
                Q_EMIT superKeyTapped();
            }
            /* Otherwise the user just terminated a hold. */
            else if(m_superKeyHeld){
                m_superKeyHeld = false;
                Q_EMIT superKeyHeldChanged(m_superKeyHeld);
            }
        }
    }
}

void
ShellDeclarativeView::ignoreSuperPress()
{
    /* There was a key pressed, ignore current super tap/hold */
    m_superPressIgnored = true;
}

void
ShellDeclarativeView::removeFocus()
{
    forceDeactivateWindow();
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

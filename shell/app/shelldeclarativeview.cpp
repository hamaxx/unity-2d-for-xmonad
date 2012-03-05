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
#include <hotmodifier.h>
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

static const char* COMMANDS_LENS_ID = "commands.lens";

static const int DASH_MIN_SCREEN_WIDTH = 1280;
static const int DASH_MIN_SCREEN_HEIGHT = 1084;

ShellDeclarativeView::ShellDeclarativeView()
    : Unity2DDeclarativeView()
    , m_mode(DesktopMode)
    , m_expanded(true)
    , m_active(false)
    , m_dashAlwaysFullScreen(false)
{
    setAttribute(Qt::WA_X11NetWmWindowTypeDock, true);
    setTransparentBackground(QX11Info::isCompositingManagerRunning());

    m_screenInfo = new ScreenInfo(ScreenInfo::TopLeft, this);

    connect(&launcher2dConfiguration(), SIGNAL(superKeyEnableChanged(bool)), SLOT(updateSuperKeyMonitoring()));
    updateSuperKeyMonitoring();

    /* Super tap shows the dash, super held shows the launcher hints */
    m_superHotModifier = KeyboardModifiersMonitor::instance()->getHotModifierFor(Qt::MetaModifier);
    connect(m_superHotModifier, SIGNAL(tapped()), SLOT(toggleDash()));
    connect(m_superHotModifier, SIGNAL(heldChanged(bool)), SIGNAL(superKeyHeldChanged(bool)));

    /* Alt tap shows the HUD */
    m_altHotModifier = KeyboardModifiersMonitor::instance()->getHotModifierFor(Qt::AltModifier);
    connect(m_altHotModifier, SIGNAL(tapped()), SIGNAL(toggleHud()));

    /* Alt+F1 reveal the launcher and gives the keyboard focus to the Dash Button. */
    Hotkey* altF1 = HotkeyMonitor::instance().getHotkeyFor(Qt::Key_F1, Qt::AltModifier);
    connect(altF1, SIGNAL(pressed()), SLOT(onAltF1Pressed()));

    /* Alt+F2 shows the dash with the commands lens activated. */
    Hotkey* altF2 = HotkeyMonitor::instance().getHotkeyFor(Qt::Key_F2, Qt::AltModifier);
    connect(altF2, SIGNAL(pressed()), SLOT(showCommandsLens()));

    /* Super+{n} for 0 ≤ n ≤ 9 activates the item with index (n + 9) % 10. */
    for (Qt::Key key = Qt::Key_0; key <= Qt::Key_9; key = (Qt::Key) (key + 1)) {
        Hotkey* hotkey = HotkeyMonitor::instance().getHotkeyFor(key, Qt::MetaModifier);
        connect(hotkey, SIGNAL(pressed()), SLOT(forwardNumericHotkey()));
        hotkey = HotkeyMonitor::instance().getHotkeyFor(key, Qt::MetaModifier | Qt::ShiftModifier);
        connect(hotkey, SIGNAL(pressed()), SLOT(forwardNumericHotkey()));
    }

    connect(m_screenInfo, SIGNAL(availableGeometryChanged(QRect)), SLOT(updateShellPosition()));
    updateShellPosition();

    // FIXME: we need to use a queued connection here otherwise QConf will deadlock for some reason
    // when we read any property from the slot (which we need to do). We need to check why this
    // happens and report a bug to dconf-qt to get it fixed.
    connect(&unity2dConfiguration(), SIGNAL(formFactorChanged(QString)),
                                     SLOT(updateDashAlwaysFullScreen()), Qt::QueuedConnection);
    connect(QApplication::desktop(), SIGNAL(resized(int)), SLOT(updateDashAlwaysFullScreen()));

    updateDashAlwaysFullScreen();
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

bool ShellDeclarativeView::dashAlwaysFullScreen() const
{
    return m_dashAlwaysFullScreen;
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
ShellDeclarativeView::onAltF1Pressed()
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

static QSize minimumSizeForDesktop()
{
    return QSize(DASH_MIN_SCREEN_WIDTH, DASH_MIN_SCREEN_HEIGHT);
}

void ShellDeclarativeView::updateDashAlwaysFullScreen()
{
    bool dashAlwaysFullScreen;
    if (unity2dConfiguration().property("formFactor").toString() != "desktop") {
        dashAlwaysFullScreen = true;
    } else {
        const QRect rect = m_screenInfo->geometry();
        const QSize minSize = minimumSizeForDesktop();
        dashAlwaysFullScreen = rect.width() < minSize.width() && rect.height() < minSize.height();
    }

    if (m_dashAlwaysFullScreen != dashAlwaysFullScreen) {
        m_dashAlwaysFullScreen = dashAlwaysFullScreen;
        Q_EMIT dashAlwaysFullScreenChanged(dashAlwaysFullScreen);
    }
}

void
ShellDeclarativeView::updateSuperKeyMonitoring()
{
    KeyboardModifiersMonitor *modifiersMonitor = KeyboardModifiersMonitor::instance();
    HotkeyMonitor& hotkeyMonitor = HotkeyMonitor::instance();

    QVariant value = launcher2dConfiguration().property("superKeyEnable");
    if (!value.isValid() || value.toBool() == true) {
        hotkeyMonitor.enableModifiers(Qt::MetaModifier);
        modifiersMonitor->enableModifiers(Qt::MetaModifier);
    } else {
        hotkeyMonitor.disableModifiers(Qt::MetaModifier);
        modifiersMonitor->disableModifiers(Qt::MetaModifier);
    }
}

void
ShellDeclarativeView::forwardNumericHotkey()
{
    Hotkey* hotkey = qobject_cast<Hotkey*>(sender());
    if (hotkey != NULL) {
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

bool
ShellDeclarativeView::superKeyHeld() const
{
    return m_superHotModifier->held();
}

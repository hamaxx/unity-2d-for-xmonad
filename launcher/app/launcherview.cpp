/*
 * Copyright (C) 2010 Canonical, Ltd.
 *
 * Authors:
 *  Olivier Tilloy <olivier.tilloy@canonical.com>
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

#include "launcherview.h"

#include <QApplication>
#include <QDesktopWidget>
#include <QX11Info>
#include <QDebug>

#include <QtDeclarative/qdeclarative.h>
#include <QDeclarativeEngine>
#include <QDeclarativeContext>
#include <QDeclarativeImageProvider>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusPendingCall>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusConnectionInterface>

#include <X11/Xlib.h>
#include <X11/Xatom.h>

// libdconf-qt
#include "qconf.h"

#include <keyboardmodifiersmonitor.h>
#include <hotkey.h>
#include <hotkeymonitor.h>
#include <keymonitor.h>
#include <debug_p.h>

static const int KEY_HOLD_THRESHOLD = 250;

static const char* DASH_DBUS_SERVICE = "com.canonical.Unity2d.Dash";
static const char* DASH_DBUS_PATH = "/Dash";
static const char* DASH_DBUS_INTERFACE = "com.canonical.Unity2d.Dash";
static const char* SPREAD_DBUS_SERVICE = "com.canonical.Unity2d.Spread";
static const char* SPREAD_DBUS_PATH = "/Spread";
static const char* SPREAD_DBUS_INTERFACE = "com.canonical.Unity2d.Spread";

static const char* DASH_DBUS_PROPERTY_ACTIVE = "active";
static const char* DASH_DBUS_METHOD_ACTIVATE_HOME = "activateHome";
static const char* SPREAD_DBUS_METHOD_IS_SHOWN = "IsShown";
static const char* COMMANDS_LENS_ID = "commands.lens";
static const char* LAUNCHER_DCONF_SCHEMA = "com.canonical.Unity2d.Launcher";

LauncherView::LauncherView(QWidget* parent) :
    Unity2DDeclarativeView(parent),
    m_superKeyPressed(false), m_superKeyHeld(false)
{
    setTransparentBackground(QX11Info::isCompositingManagerRunning());

    m_superKeyHoldTimer.setSingleShot(true);
    m_superKeyHoldTimer.setInterval(KEY_HOLD_THRESHOLD);
    connect(&m_superKeyHoldTimer, SIGNAL(timeout()), SLOT(updateSuperKeyHoldState()));
    connect(this, SIGNAL(superKeyTapped()), SLOT(toggleDash()));

    m_dconf_launcher = new QConf(LAUNCHER_DCONF_SCHEMA);
    connect(m_dconf_launcher, SIGNAL(superKeyEnableChanged(bool)), SLOT(updateSuperKeyMonitoring()));
    updateSuperKeyMonitoring();

    /* Alt+F1 gives the keyboard focus to the launcher. */
    Hotkey* altF1 = HotkeyMonitor::instance().getHotkeyFor(Qt::Key_F1, Qt::AltModifier);
    connect(altF1, SIGNAL(pressed()), SLOT(forceActivateWindow()));

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
}

LauncherView::~LauncherView()
{
    delete m_dconf_launcher;
}

void
LauncherView::focusInEvent(QFocusEvent* event)
{
    QDeclarativeView::focusInEvent(event);
    Q_EMIT focusChanged(true);
}

void
LauncherView::focusOutEvent(QFocusEvent* event)
{
    QDeclarativeView::focusOutEvent(event);
    Q_EMIT focusChanged(false);
}

void
LauncherView::updateSuperKeyMonitoring()
{
    KeyboardModifiersMonitor *modifiersMonitor = KeyboardModifiersMonitor::instance();
    KeyMonitor *keyMonitor = KeyMonitor::instance();

    QVariant value = m_dconf_launcher->property("superKeyEnable");
    if (!value.isValid() || value.toBool() == true) {
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
LauncherView::setHotkeysForModifiers(Qt::KeyboardModifiers modifiers)
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
LauncherView::updateSuperKeyHoldState()
{
    /* If the key was released in the meantime, just do nothing, otherwise
       consider the key being held, unless we're told to ignore it. */
    if (m_superKeyPressed && !m_superPressIgnored) {
        m_superKeyHeld = true;
        Q_EMIT superKeyHeldChanged(m_superKeyHeld);
    }
}

void
LauncherView::ignoreSuperPress()
{
    /* There was a key pressed, ignore current super tap/hold */
    m_superPressIgnored = true;
}

void
LauncherView::forwardNumericHotkey()
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

void
LauncherView::toggleDash()
{
    QDBusInterface dashInterface(DASH_DBUS_SERVICE, DASH_DBUS_PATH, DASH_DBUS_INTERFACE);

    QVariant dashActiveResult = dashInterface.property(DASH_DBUS_PROPERTY_ACTIVE);
    if (!dashActiveResult.isValid()) {
        UQ_WARNING << "Can't read the DBUS Dash property" << DASH_DBUS_PROPERTY_ACTIVE
                   << "on" << DASH_DBUS_SERVICE << DASH_DBUS_PATH << DASH_DBUS_INTERFACE;
        return;
    }

    bool dashActive = dashActiveResult.toBool();
    if (dashActive) {
        if (!dashInterface.setProperty(DASH_DBUS_PROPERTY_ACTIVE, false)) {
            UQ_WARNING << "Can't set the DBUS Dash property" << DASH_DBUS_PROPERTY_ACTIVE
                       << "on" << DASH_DBUS_SERVICE << DASH_DBUS_PATH << DASH_DBUS_INTERFACE;
        }
    } else {
        /* Check if the spread is active before activating the dash.
           We need to do this since the spread can't prevent the launcher from
           monitoring the super key and therefore getting to this point if
           it's tapped. */

        /* Check if the spread is present on DBUS first, as we don't want to have DBUS
           activate it if it's not running yet */
        QDBusConnectionInterface* sessionBusIFace = QDBusConnection::sessionBus().interface();
        QDBusReply<bool> reply = sessionBusIFace->isServiceRegistered(SPREAD_DBUS_SERVICE);
        if (reply.isValid() && reply.value() == true) {
            QDBusInterface spreadInterface(SPREAD_DBUS_SERVICE, SPREAD_DBUS_PATH,
                                           SPREAD_DBUS_INTERFACE);

            QDBusReply<bool> spreadActiveResult = spreadInterface.call(SPREAD_DBUS_METHOD_IS_SHOWN);
            if (spreadActiveResult.isValid() && spreadActiveResult.value() == true) {
                return;
            }
        }

        dashInterface.asyncCall(DASH_DBUS_METHOD_ACTIVATE_HOME);
    }
}

void
LauncherView::showCommandsLens()
{
    QDBusInterface dashInterface(DASH_DBUS_SERVICE, DASH_DBUS_PATH, DASH_DBUS_INTERFACE);
    dashInterface.asyncCall("activateLens", COMMANDS_LENS_ID);
}


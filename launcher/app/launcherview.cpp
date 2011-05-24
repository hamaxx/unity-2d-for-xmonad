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

#include <keyboardmodifiersmonitor.h>
#include <hotkey.h>
#include <hotkeymonitor.h>
#include <dragdropevent.h>
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
static const char* APPLICATIONS_PLACE = "/usr/share/unity/places/applications.place";
static const char* COMMANDS_PLACE_ENTRY = "Runner";

LauncherView::LauncherView(QWidget* parent) :
    Unity2DDeclarativeView(parent),
    m_superKeyPressed(false), m_superKeyHeld(false)
{
    if (QX11Info::isCompositingManagerRunning()) {
        setAttribute(Qt::WA_TranslucentBackground);
        viewport()->setAttribute(Qt::WA_TranslucentBackground);
    } else {
        setAttribute(Qt::WA_OpaquePaintEvent);
        setAttribute(Qt::WA_NoSystemBackground);
    }

    m_superKeyHoldTimer.setSingleShot(true);
    m_superKeyHoldTimer.setInterval(KEY_HOLD_THRESHOLD);
    connect(&m_superKeyHoldTimer, SIGNAL(timeout()), SLOT(updateSuperKeyHoldState()));
    connect(this, SIGNAL(superKeyTapped()), SLOT(toggleDash()));

    m_enableSuperKey.setKey("/desktop/unity-2d/launcher/super_key_enable");
    connect(&m_enableSuperKey, SIGNAL(valueChanged()), SLOT(updateSuperKeyMonitoring()));
    updateSuperKeyMonitoring();

    /* Alt+F1 gives the keyboard focus to the launcher. */
    Hotkey* altF1 = HotkeyMonitor::instance().getHotkeyFor(Qt::Key_F1, Qt::AltModifier);
    connect(altF1, SIGNAL(pressed()), SLOT(activateWindow()));

    /* Alt+F2 shows the dash with the commands place entry activated. */
    Hotkey* altF2 = HotkeyMonitor::instance().getHotkeyFor(Qt::Key_F2, Qt::AltModifier);
    connect(altF2, SIGNAL(pressed()), SLOT(showCommandsPlace()));

    /* Super+{n} for 0 ≤ n ≤ 9 activates the item with index (n + 9) % 10. */
    for (Qt::Key key = Qt::Key_0; key <= Qt::Key_9; key = (Qt::Key) (key + 1)) {
        Hotkey* hotkey = HotkeyMonitor::instance().getHotkeyFor(key, Qt::MetaModifier);
        connect(hotkey, SIGNAL(pressed()), SLOT(forwardNumericHotkey()));
    }
}

void
LauncherView::activateWindow()
{
    QDeclarativeView::activateWindow();
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

    QVariant value = m_enableSuperKey.getValue();
    if (!value.isValid() || value.toBool() == true) {
        QObject::connect(modifiersMonitor,
                         SIGNAL(keyboardModifiersChanged(Qt::KeyboardModifiers)),
                         this, SLOT(setHotkeysForModifiers(Qt::KeyboardModifiers)));
        setHotkeysForModifiers(modifiersMonitor->keyboardModifiers());
    } else {
        QObject::disconnect(modifiersMonitor,
                            SIGNAL(keyboardModifiersChanged(Qt::KeyboardModifiers)),
                            this, SLOT(setHotkeysForModifiers(Qt::KeyboardModifiers)));
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
            /* If the key is pressed, start up a timer to monitor if it's being held short
               enough to qualify as just a "tap" or as a proper hold */
            m_superKeyHoldTimer.start();
        } else {
            m_superKeyHoldTimer.stop();

            /* If the key is released, and was not being held, it means that the user just
               performed a "tap". Otherwise the user just terminated a hold. */
            if (!m_superKeyHeld) {
                Q_EMIT superKeyTapped();
            } else {
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
       consider the key being held. */
    if (m_superKeyPressed) {
        m_superKeyHeld = true;
        Q_EMIT superKeyHeldChanged(m_superKeyHeld);
    }
}

void
LauncherView::forwardNumericHotkey()
{
    Hotkey* hotkey = qobject_cast<Hotkey*>(sender());
    if (hotkey != NULL) {
        /* Shortcuts from 1 to 9 should activate the items with index
           from 0 to 8. Shortcut for 0 should activate item with index 9.
           In other words, the indexes are activated in the same order as
           the keys appear on a standard keyboard. */
        Qt::Key key = hotkey->key();
        if (key >= Qt::Key_0 && key <= Qt::Key_9) {
            int index = (key - Qt::Key_0 + 9) % 10;
            Q_EMIT keyboardShortcutPressed(index);
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

QList<QUrl>
LauncherView::getEventUrls(DeclarativeDragDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasUrls()) {
        return mimeData->urls();
    }
    else if (mimeData->hasText()) {
        /* When dragging an URL from firefox’s address bar, it is properly
           recognized as such by the event. However, the same doesn’t work
           for chromium: the URL is recognized as plain text.
           We cope with this unfriendly behaviour by trying to build a URL out
           of the text. This assumes there’s only one URL. */
        QString text = mimeData->text();
        QUrl url(text);
        if (url.isRelative()) {
            /* On top of that, chromium sometimes chops off the scheme… */
            url = QUrl("http://" + text);
        }
        if (url.isValid()) {
            QList<QUrl> urls;
            urls.append(url);
            return urls;
        }
    }
    return QList<QUrl>();
}

void LauncherView::onDragEnter(DeclarativeDragDropEvent* event)
{
    Q_FOREACH(QUrl url, getEventUrls(event)) {
        if ((url.scheme() == "file" && url.path().endsWith(".desktop")) ||
            url.scheme().startsWith("http")) {
            event->setAccepted(true);
            return;
        }
    }
}

void LauncherView::onDrop(DeclarativeDragDropEvent* event)
{
    foreach (QUrl url, getEventUrls(event)) {
        if (url.scheme() == "file" && url.path().endsWith(".desktop")) {
            emit desktopFileDropped(url.path());
        }
        else if (url.scheme().startsWith("http")) {
            emit webpageUrlDropped(url);
        }
    }
}

/* Calculates both the background color and the glow color of a launcher tile
   based on the colors in the specified icon (using the same algorithm as Unity).
   The values are returned as list where the first item is the background color
   and the second one is the glow color.
*/
QList<QVariant>
LauncherView::getColorsFromIcon(QUrl source, QSize size) const
{
    QList<QVariant> colors;

    // FIXME: we should find a way to avoid reloading the icon
    QImage icon = engine()->imageProvider("icons")->requestImage(source.path().mid(1), &size, size);
    if (icon.width() == 0 || icon.height() == 0) {
        UQ_WARNING << "Unable to load icon in getColorsFromIcon from" << source;
        return colors;
    }

    long int rtotal = 0, gtotal = 0, btotal = 0;
    float total = 0.0f;

    for (int y = 0; y < icon.height(); ++y) {
        for (int x = 0; x < icon.width(); ++x) {
            QColor color = QColor::fromRgba(icon.pixel(x, y));

            float saturation = (qMax (color.red(), qMax (color.green(), color.blue())) -
                                qMin (color.red(), qMin (color.green(), color.blue()))) / 255.0f;
            float relevance = .1 + .9 * (color.alpha() / 255.0f) * saturation;

            rtotal += (unsigned char) (color.red() * relevance);
            gtotal += (unsigned char) (color.green() * relevance);
            btotal += (unsigned char) (color.blue() * relevance);

            total += relevance * 255;
        }
    }

    QColor hsv = QColor::fromRgbF(rtotal / total, gtotal / total, btotal / total).toHsv();

    /* Background color is the base color with 0.90f HSV value */
    hsv.setHsvF(hsv.hueF(),
                (hsv.saturationF() > .15f) ? 0.65f : hsv.saturationF(),
                0.90f);
    colors.append(QVariant::fromValue(hsv.toRgb()));

    /* Glow color is the base color with 1.0f HSV value */
    hsv.setHsvF(hsv.hueF(), hsv.saturationF(), 1.0f);
    colors.append(QVariant::fromValue(hsv.toRgb()));

    return colors;
}

void
LauncherView::showCommandsPlace()
{
    QDBusInterface dashInterface(DASH_DBUS_SERVICE, DASH_DBUS_PATH, DASH_DBUS_INTERFACE);
    dashInterface.asyncCall("activatePlaceEntry",
                            APPLICATIONS_PLACE, COMMANDS_PLACE_ENTRY, 0);
}


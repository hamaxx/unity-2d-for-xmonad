/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
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
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#include "shellmanager.h"

// Qt
#include <QApplication>
#include <QDebug>
#include <QtDeclarative>
#include <QDeclarativeEngine>
#include <QDeclarativeView>
#include <QDesktopWidget>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QDeclarativeContext>
#include <QAbstractEventDispatcher>

// libunity-2d-private
#include <hotkeymonitor.h>
#include <hotkey.h>
#include <keyboardmodifiersmonitor.h>
#include <keymonitor.h>
#include <screeninfo.h>

// Local
#include "shelldeclarativeview.h"
#include "dashclient.h"
#include "dashdbus.h"
#include "gesturehandler.h"
#include "config.h"

// unity-2d
#include <unity2ddebug.h>
#include <unity2dapplication.h>

// X11
#include <X11/Xlib.h>

static const int KEY_HOLD_THRESHOLD = 250;

struct ShellManagerPrivate
{
    ShellManagerPrivate()
        : q(0)
        , m_dashDBus(0)
        , m_superKeyPressed(false)
        , m_superKeyHeld(false)
    {}

    ShellDeclarativeView* initShell(bool isTopLeft, int screen);
    void updateScreenCount(int newCount);
    ShellDeclarativeView* activeShell() const;

    ShellManager *q;
    QList<ShellDeclarativeView *> m_viewList;
    DashDBus * m_dashDBus;
    QUrl m_sourceFileUrl;

    bool m_superKeyPressed;
    bool m_superKeyHeld;
    bool m_superPressIgnored;
    QTimer m_superKeyHoldTimer;
};


ShellDeclarativeView *
ShellManagerPrivate::initShell(bool isTopLeft, int screen)
{
    const QStringList arguments = qApp->arguments();
    ShellDeclarativeView * view = new ShellDeclarativeView(m_sourceFileUrl, isTopLeft, screen);
    view->setAccessibleName("Shell");
    if (arguments.contains("-opengl")) {
        view->setUseOpenGL(true);
    }

    Unity2dApplication::instance()->installX11EventFilter(view);

    view->engine()->addImportPath(unity2dImportPath());
    view->engine()->setBaseUrl(QUrl::fromLocalFile(unity2dDirectory() + "/shell/"));

    /* Load the QML UI, focus and show the window */
    view->setResizeMode(QDeclarativeView::SizeViewToRootObject);
    view->rootContext()->setContextProperty("declarativeView", view);
    view->rootContext()->setContextProperty("shellManager", q);
    // WARNING This declaration of dashClient used to be in Unity2d/plugin.cpp
    // but it lead to locks when both the shell and the spread were started
    // at the same time since SpreadMonitor QDBusServiceWatcher::serviceRegistered
    // and DashClient QDBusServiceWatcher::serviceRegistered
    // triggered at the same time ending up with both creating QDBusInterface
    // to eachother in the main thread meaning they would block
    // In case you need to have a DashClient in the spread the fix for the problem
    // is moving the QDbusInterface creation to a thread so it does not block
    // the main thread
    view->rootContext()->setContextProperty("dashClient", DashClient::instance());

    if (!m_dashDBus) {
        m_dashDBus = new DashDBus(view);
        if (!m_dashDBus->connectToBus()) {
            qCritical() << "Another instance of the Dash already exists. Quitting.";
            return 0;
        }
    }

    view->show();

    return view;
}

ShellDeclarativeView *
ShellManagerPrivate::activeShell() const
{
    int cursorScreen = ScreenInfo::cursorScreen();
    Q_FOREACH(ShellDeclarativeView * shell, m_viewList) {
        if (shell->screenNumber() == cursorScreen) {
            return shell;
        }
    }
    return 0;
}

void
ShellManagerPrivate::updateScreenCount(int newCount)
{
    if (newCount > 0) {
        QDesktopWidget* desktop = QApplication::desktop();
        int size = m_viewList.size();
        ShellDeclarativeView* shell = 0;

        /* The first shell is always the one on the leftmost screen. */
        QPoint p;
        if (QApplication::isRightToLeft()) {
            p = QPoint(desktop->width() - 1, 0);
        }
        int leftmost = desktop->screenNumber(p);
        if (size > 0) {
            shell = m_viewList[0];
        } else {
            shell = initShell(true, leftmost);
            m_viewList.append(shell);
        }
        shell->setScreenNumber(leftmost);

        /* Update the position of other existing Shells, and instantiate new
           Shells as needed. */
        int i = 1;
        for (int screen = 0; screen < newCount; ++screen) {
            if (screen == leftmost) {
                continue;
            }
            if (i < size) {
                shell = m_viewList[i];
            } else {
                shell = initShell(false, screen);
                m_viewList.append(shell);
            }
            shell->setIsTopLeftShell(false);
            shell->setScreenNumber(screen);
            ++i;
        }
    }
    /* Remove extra Shells if any. */
    while (m_viewList.size() > newCount) {
        m_viewList.takeLast()->deleteLater();
    }
}

/* -------------------------- ShellManager -----------------------------*/

ShellManager::ShellManager(const QUrl &sourceFileUrl, QObject* parent) :
    QObject(parent)
    ,d(new ShellManagerPrivate)
{
    d->q = this;
    d->m_sourceFileUrl = sourceFileUrl;

    qmlRegisterType<ShellDeclarativeView>("Unity2d", 1, 0, "ShellDeclarativeView");

    QDesktopWidget* desktop = QApplication::desktop();

    d->updateScreenCount(desktop->screenCount());

    connect(desktop, SIGNAL(screenCountChanged(int)), SLOT(onScreenCountChanged(int)));

    d->m_superKeyHoldTimer.setSingleShot(true);
    d->m_superKeyHoldTimer.setInterval(KEY_HOLD_THRESHOLD);
    connect(&d->m_superKeyHoldTimer, SIGNAL(timeout()), SLOT(updateSuperKeyHoldState()));

    connect(&launcher2dConfiguration(), SIGNAL(superKeyEnableChanged(bool)), SLOT(updateSuperKeyMonitoring()));
    updateSuperKeyMonitoring();

    /* Alt+F1 reveal the launcher and gives the keyboard focus to the Dash Button. */
    Hotkey* altF1 = HotkeyMonitor::instance().getHotkeyFor(Qt::Key_F1, Qt::AltModifier);
    connect(altF1, SIGNAL(pressed()), SLOT(onAltF1Pressed()));

    /* Alt+F2 shows the dash with the commands lens activated. */
    Hotkey* altF2 = HotkeyMonitor::instance().getHotkeyFor(Qt::Key_F2, Qt::AltModifier);
    connect(altF2, SIGNAL(pressed()), SLOT(onAltF2Pressed()));

    /* Super+{n} for 0 ≤ n ≤ 9 activates the item with index (n + 9) % 10. */
    for (Qt::Key key = Qt::Key_0; key <= Qt::Key_9; key = (Qt::Key) (key + 1)) {
        Hotkey* hotkey = HotkeyMonitor::instance().getHotkeyFor(key, Qt::MetaModifier);
        connect(hotkey, SIGNAL(pressed()), SLOT(onNumericHotkeyPressed()));
        hotkey = HotkeyMonitor::instance().getHotkeyFor(key, Qt::MetaModifier | Qt::ShiftModifier);
        connect(hotkey, SIGNAL(pressed()), SLOT(onNumericHotkeyPressed()));
    }
}

ShellManager::~ShellManager()
{
    qDeleteAll(d->m_viewList);
    delete d;
}

void
ShellManager::onScreenCountChanged(int newCount)
{
    d->updateScreenCount(newCount);
}

/* ----------------- super key handling ---------------- */

void
ShellManager::updateSuperKeyHoldState()
{
    /* If the key was released in the meantime, just do nothing, otherwise
       consider the key being held, unless we're told to ignore it. */
    if (d->m_superKeyPressed && !d->m_superPressIgnored) {
        d->m_superKeyHeld = true;
        Q_EMIT superKeyHeldChanged(d->m_superKeyHeld);
    }
}

void
ShellManager::updateSuperKeyMonitoring()
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
        d->m_superKeyHoldTimer.stop();
        d->m_superKeyPressed = false;
        if (d->m_superKeyHeld) {
            d->m_superKeyHeld = false;
            Q_EMIT superKeyHeldChanged(false);
        }
    }
}

void
ShellManager::setHotkeysForModifiers(Qt::KeyboardModifiers modifiers)
{
    /* This is the new new state of the Super key (AKA Meta key), while
       d->m_superKeyPressed is the previous state of the key at the last modifiers change. */
    bool superKeyPressed = modifiers.testFlag(Qt::MetaModifier);

    if (d->m_superKeyPressed != superKeyPressed) {
        d->m_superKeyPressed = superKeyPressed;
        if (superKeyPressed) {
            d->m_superPressIgnored = false;
            /* If the key is pressed, start up a timer to monitor if it's being held short
               enough to qualify as just a "tap" or as a proper hold */
            d->m_superKeyHoldTimer.start();
        } else {
            d->m_superKeyHoldTimer.stop();

            /* If the key is released, and was not being held, it means that the user just
               performed a "tap". Unless we're told to ignore that tap, that is. */
            if (!d->m_superKeyHeld && !d->m_superPressIgnored) {
                onSuperKeyTapped();
            }
            /* Otherwise the user just terminated a hold. */
            else if(d->m_superKeyHeld){
                d->m_superKeyHeld = false;
                Q_EMIT superKeyHeldChanged(d->m_superKeyHeld);
            }
        }
    }
}

void
ShellManager::ignoreSuperPress()
{
    /* There was a key pressed, ignore current super tap/hold */
    d->m_superPressIgnored = true;
}

void
ShellManager::onSuperKeyTapped()
{
    // TODO : In future, All shells should be able to handle the Dash
    // Note: Always, first item in the list is the topLeft shell
    // In any case, just iterate through the list
    Q_FOREACH(ShellDeclarativeView *shell, d->m_viewList) {
        if (shell->isTopLeftShell()) {
            shell->toggleDash();
            break;
        }
    }
}

bool
ShellManager::superKeyHeld() const
{
    return d->m_superKeyHeld;
}

/*------------------ Hotkeys Handling -----------------------*/

void
ShellManager::onAltF1Pressed()
{
    ShellDeclarativeView * activeShell = d->activeShell();
    if (activeShell) {
        activeShell->toggleLauncher();
    }
}

void
ShellManager::onAltF2Pressed()
{
    ShellDeclarativeView * activeShell = d->activeShell();
    if (activeShell) {
        activeShell->showCommandsLens();
    }
}

void
ShellManager::onNumericHotkeyPressed()
{
    Hotkey* hotkey = qobject_cast<Hotkey*>(sender());
    if (hotkey) {
        ShellDeclarativeView * activeShell = d->activeShell();
        if (activeShell) {
            activeShell->processNumericHotkey(hotkey);
        }
    }
}

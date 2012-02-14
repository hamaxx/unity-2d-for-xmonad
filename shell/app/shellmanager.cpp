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
#include <screeninfo.h>

// Local
#include "shelldeclarativeview.h"
#include "dashclient.h"
#include "dashdbus.h"
#include "gesturehandler.h"
#include "launcherdbus.h"
#include "config.h"

// unity-2d
#include <unity2ddebug.h>
#include <unity2dapplication.h>

// X11
#include <X11/Xlib.h>

struct ShellManagerPrivate
{
    ShellManagerPrivate() :
        q(0), m_dashDBus(0), m_launcherDBus(0), m_previousActiveShell(0)
    {}

    ShellDeclarativeView* initShell(bool isTopLeft, int screen);
    void updateScreenCount(int newCount);
    ShellDeclarativeView* activeShell() const;

    ShellManager *q;
    QList<ShellDeclarativeView *> m_viewList;
    DashDBus * m_dashDBus;
    LauncherDBus* m_launcherDBus;
    ShellDeclarativeView * m_previousActiveShell;
    QUrl m_sourceFileUrl;
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

    if (!m_launcherDBus) {
        m_launcherDBus = new LauncherDBus(view);
        m_launcherDBus->connectToBus();
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

/*------------------ Hotkeys Handling -----------------------*/

void
ShellManager::onAltF1Pressed()
{
    ShellDeclarativeView * activeShell = d->activeShell();
    // Note: Check whether the previous shell has active focus still
    // and remove its focus
    if (d->m_previousActiveShell != 0 && activeShell != d->m_previousActiveShell) {
        if (d->m_previousActiveShell->isActiveWindow()) {
            d->m_previousActiveShell->toggleLauncher();
        }
    }
    if (activeShell) {
        activeShell->toggleLauncher();
    }

    d->m_previousActiveShell = activeShell;
}

void
ShellManager::onAltF2Pressed()
{
    d->m_previousActiveShell = d->activeShell();
    if (d->m_previousActiveShell) {
        d->m_previousActiveShell->showCommandsLens();
    }
}

void
ShellManager::onNumericHotkeyPressed()
{
    Hotkey* hotkey = qobject_cast<Hotkey*>(sender());
    if (hotkey) {
        d->m_previousActiveShell = d->activeShell();
        if (d->m_previousActiveShell) {
            d->m_previousActiveShell->processNumericHotkey(hotkey);
        }
    }
}

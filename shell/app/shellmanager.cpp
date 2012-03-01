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
#include <QDesktopWidget>

// libunity-2d-private
#include <hotmodifier.h>
#include <hotkeymonitor.h>
#include <hotkey.h>
#include <keyboardmodifiersmonitor.h>
#include <keymonitor.h>
#include <screeninfo.h>

// Local
#include "shelldeclarativeview.h"
#include "config.h"

// unity-2d
#include <unity2ddebug.h>

static const char* COMMANDS_LENS_ID = "commands.lens";
static const int DASH_MIN_SCREEN_WIDTH = 1280;
static const int DASH_MIN_SCREEN_HEIGHT = 1084;

struct ShellManagerPrivate
{
    ShellManagerPrivate()
        : q(NULL)
        , m_shellWithDash(NULL)
        , m_shellWithHud(NULL)
        , m_hudLoader(NULL)
        , m_dashAlwaysFullScreen(false)
        , m_dashActive(false)
        , m_dashMode(ShellManager::DesktopMode)
        , m_superHotModifier(NULL)
    {}

    ShellDeclarativeView* initShell(int screen);
    void updateScreenCount(int newCount);
    ShellDeclarativeView* activeShell() const;
    void moveDashToShell(ShellDeclarativeView* newShell);
    void moveHudToShell(ShellDeclarativeView* newShell);

    ShellManager *q;
    QList<ShellDeclarativeView *> m_viewList;
    ShellDeclarativeView * m_shellWithDash;
    ShellDeclarativeView * m_shellWithHud;
    QDeclarativeItem* m_hudLoader;
    bool m_dashAlwaysFullScreen;
    QUrl m_sourceFileUrl;
    bool m_dashActive;
    ShellManager::DashMode m_dashMode;
    QString m_dashActiveLens; /* Lens id of the active lens */

    HotModifier* m_superHotModifier;
    HotModifier* m_altHotModifier;
};


ShellDeclarativeView *
ShellManagerPrivate::initShell(int screen)
{
    const QStringList arguments = qApp->arguments();
    ShellDeclarativeView * view = new ShellDeclarativeView(m_sourceFileUrl, screen);
    view->setAccessibleName("Shell");
    if (arguments.contains("-opengl")) {
        view->setUseOpenGL(true);
    }

    Unity2dApplication::instance()->installX11EventFilter(view);

    view->engine()->addImportPath(unity2dImportPath());
    view->engine()->setBaseUrl(QUrl::fromLocalFile(unity2dDirectory() + "/shell/"));

    view->rootContext()->setContextProperty("shellManager", q);

    view->show();

    return view;
}

ShellDeclarativeView *
ShellManagerPrivate::activeShell() const
{
    int cursorScreen = QApplication::desktop()->screenNumber(QCursor::pos());
    Q_FOREACH(ShellDeclarativeView * shell, m_viewList) {
        if (shell->screen()->screen() == cursorScreen) {
            return shell;
        }
    }
    return 0;
}

void
ShellManagerPrivate::updateScreenCount(int newCount)
{
    const int previousCount = m_viewList.size();

    /* Update the position of other existing Shells, and instantiate new Shells as needed. */
    for (int screen = previousCount; screen < newCount; ++screen) {
        ShellDeclarativeView *shell = initShell(screen);
        m_viewList.append(shell);

        if (screen == 0) {
            m_shellWithDash = m_viewList[0];
            Q_EMIT q->dashShellChanged(m_shellWithDash);
            Q_EMIT q->dashScreenChanged(q->dashScreen());

            m_shellWithHud = m_viewList[0];
            Q_EMIT q->hudShellChanged(m_shellWithHud);
            Q_EMIT q->hudScreenChanged(q->hudScreen());

            m_hudLoader = qobject_cast<QDeclarativeItem*>(m_shellWithHud->rootObject()->property("hudLoader").value<QObject *>());
            QObject::connect(m_hudLoader, SIGNAL(activeChanged()), q, SIGNAL(hudActiveChanged()));
        }
    }

    /* Remove extra Shells if any. */
    while (m_viewList.size() > newCount) {
        ShellDeclarativeView *shell = m_viewList.takeLast();
        if (shell == m_shellWithDash) {
            if (newCount > 0) {
                moveDashToShell(m_viewList[0]);
            }
        }
        if (shell == m_shellWithHud) {
            if (newCount > 0) {
                moveHudToShell(m_viewList[0]);
            }
        }
        shell->deleteLater();
    }

}

static QList<QDeclarativeItem *> dumpFocusedItems(QObject *obj) {
    QList<QDeclarativeItem *> res;
    QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(obj);
    if (item && item->hasFocus()) {
        res << item;
    }
    Q_FOREACH (QObject *childObj, obj->children()) {
        res += dumpFocusedItems(childObj);
    }
    return res;
}

static bool moveRootItemToShell(const char *itemName, ShellDeclarativeView* newShell, ShellDeclarativeView* oldShell)
{
    bool itemMoved = false;

    if (newShell != oldShell) {
        QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(oldShell->rootObject()->property(itemName).value<QObject *>());
        if (item) {
            const QGraphicsView::ViewportUpdateMode vum1 = oldShell->viewportUpdateMode();
            const QGraphicsView::ViewportUpdateMode vum2 = newShell->viewportUpdateMode();
            oldShell->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
            newShell->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);

            // Moving the item around makes it lose its focus values, remember them and set them later
            const QList<QDeclarativeItem *> itemChildrenFocusedItems = dumpFocusedItems(item);

            oldShell->rootObject()->setProperty(itemName, QVariant());
            oldShell->scene()->removeItem(item);

            item->setParentItem(qobject_cast<QDeclarativeItem*>(newShell->rootObject()));
            newShell->rootObject()->setProperty(itemName, QVariant::fromValue<QObject*>(item));

            Q_FOREACH(QDeclarativeItem *childrenItem, itemChildrenFocusedItems) {
                childrenItem->setFocus(true);
            }

            oldShell->setViewportUpdateMode(vum1);
            newShell->setViewportUpdateMode(vum2);

            itemMoved = true;
        } else {
            qWarning() << "moveRootItemToShell: Could not find the item" << itemName;
        }
    }

    return itemMoved;
}

void ShellManagerPrivate::moveDashToShell(ShellDeclarativeView* newShell)
{
    if (moveRootItemToShell("dashLoader", newShell, m_shellWithDash)) {
        m_shellWithDash = newShell;
        Q_EMIT q->dashShellChanged(newShell);
        Q_EMIT q->dashScreenChanged(q->dashScreen());
    }
}

void ShellManagerPrivate::moveHudToShell(ShellDeclarativeView* newShell)
{
    if (moveRootItemToShell("hudLoader", newShell, m_shellWithHud)) {
        m_shellWithHud = newShell;
        Q_EMIT q->hudShellChanged(newShell);
        Q_EMIT q->hudScreenChanged(q->hudScreen());
    }
}

/* -------------------------- ShellManager -----------------------------*/

ShellManager::ShellManager(const QUrl &sourceFileUrl, QObject* parent) :
    QObject(parent)
    ,d(new ShellManagerPrivate)
{
    d->q = this;
    d->m_sourceFileUrl = sourceFileUrl;

    qmlRegisterUncreatableType<ShellDeclarativeView>("Unity2d", 1, 0, "ShellDeclarativeView", "This can only be created from C++");
    qmlRegisterUncreatableType<ShellManager>("Unity2d", 1, 0, "ShellManager", "This can only be created from C++");

    QDesktopWidget* desktop = QApplication::desktop();

    d->updateScreenCount(desktop->screenCount());

    connect(desktop, SIGNAL(screenCountChanged(int)), SLOT(onScreenCountChanged(int)));

    connect(&launcher2dConfiguration(), SIGNAL(superKeyEnableChanged(bool)), SLOT(updateSuperKeyMonitoring()));
    updateSuperKeyMonitoring();

    /* Super tap shows the dash, super held shows the launcher hints */
    d->m_superHotModifier = KeyboardModifiersMonitor::instance()->getHotModifierFor(Qt::MetaModifier);
    connect(d->m_superHotModifier, SIGNAL(tapped()), SLOT(toggleDash()));
    connect(d->m_superHotModifier, SIGNAL(heldChanged(bool)), SIGNAL(superKeyHeldChanged(bool)));

    /* Alt tap shows the HUD */
    d->m_altHotModifier = KeyboardModifiersMonitor::instance()->getHotModifierFor(Qt::AltModifier);
    connect(d->m_altHotModifier, SIGNAL(tapped()), SLOT(toggleHudRequested()));

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

    // FIXME: we need to use a queued connection here otherwise QConf will deadlock for some reason
    // when we read any property from the slot (which we need to do). We need to check why this
    // happens and report a bug to dconf-qt to get it fixed.
    connect(&unity2dConfiguration(), SIGNAL(formFactorChanged(QString)),
                                     SLOT(updateDashAlwaysFullScreen()), Qt::QueuedConnection);
    connect(this, SIGNAL(dashShellChanged(QObject *)), this, SLOT(updateDashAlwaysFullScreen()));
    connect(QApplication::desktop(), SIGNAL(resized(int)), SLOT(updateDashAlwaysFullScreen()));

    updateDashAlwaysFullScreen();
}

ShellManager::~ShellManager()
{
    qDeleteAll(d->m_viewList);
    delete d;
}

void
ShellManager::setDashActive(bool value)
{
    if (value != d->m_dashActive) {
        d->m_dashActive = value;
        Q_EMIT dashActiveChanged(d->m_dashActive);
    }
}

bool
ShellManager::dashActive() const
{
    return d->m_dashActive;
}

void
ShellManager::setDashMode(DashMode mode)
{
    if (d->m_dashMode == mode) {
        return;
    }

    d->m_dashMode = mode;
    dashModeChanged(d->m_dashMode);
}

ShellManager::DashMode
ShellManager::dashMode() const
{
    return d->m_dashMode;
}

void
ShellManager::setDashActiveLens(const QString& activeLens)
{
    if (activeLens != d->m_dashActiveLens) {
        d->m_dashActiveLens = activeLens;
        Q_EMIT dashActiveLensChanged(activeLens);
    }
}

const QString&
ShellManager::dashActiveLens() const
{
    return d->m_dashActiveLens;
}

bool
ShellManager::dashHaveCustomHomeShortcuts() const
{
    return QFileInfo(unity2dDirectory() + "/shell/dash/HomeShortcutsCustomized.qml").exists();
}

QObject *
ShellManager::dashShell() const
{
    return d->m_shellWithDash;
}

int
ShellManager::dashScreen() const
{
    return d->m_shellWithDash->screen()->screen();
}

bool
ShellManager::dashAlwaysFullScreen() const
{
    return d->m_dashAlwaysFullScreen;
}

void
ShellManager::onScreenCountChanged(int newCount)
{
    d->updateScreenCount(newCount);
}

void
ShellManager::toggleDash()
{
    ShellDeclarativeView * activeShell = d->activeShell();
    if (activeShell) {
        const bool differentShell = d->m_shellWithDash != activeShell;

        if (dashActive() && !differentShell) {
            setDashActive(false);
            d->m_shellWithDash->forceDeactivateWindow();
        } else {
            d->moveDashToShell(activeShell);
            Q_EMIT dashActivateHome();
        }
    }
}

void
ShellManager::toggleHudRequested()
{
    ShellDeclarativeView * activeShell = d->activeShell();
    if (activeShell) {
        const bool differentShell = d->m_shellWithHud != activeShell;

        if (differentShell) {
            d->moveHudToShell(activeShell);

            if (!hudActive()) {
                Q_EMIT toggleHud();
            } else {
                activeShell->forceActivateWindow();
            }
        } else {
            Q_EMIT toggleHud();
        }
    }
}

static QSize minimumSizeForDesktop()
{
    return QSize(DASH_MIN_SCREEN_WIDTH, DASH_MIN_SCREEN_HEIGHT);
}

void ShellManager::updateDashAlwaysFullScreen()
{
    bool dashAlwaysFullScreen;
    if (unity2dConfiguration().property("formFactor").toString() != "desktop") {
        dashAlwaysFullScreen = true;
    } else {
        const QRect rect = QApplication::desktop()->screenGeometry(d->m_shellWithDash);
        const QSize minSize = minimumSizeForDesktop();
        dashAlwaysFullScreen = rect.width() < minSize.width() && rect.height() < minSize.height();
    }

    if (d->m_dashAlwaysFullScreen != dashAlwaysFullScreen) {
        d->m_dashAlwaysFullScreen = dashAlwaysFullScreen;
        Q_EMIT dashAlwaysFullScreenChanged(dashAlwaysFullScreen);
    }
}

/* ----------------- super key handling ---------------- */

void
ShellManager::updateSuperKeyMonitoring()
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

bool
ShellManager::superKeyHeld() const
{
    if (d->m_superHotModifier == NULL) // We are just initializing
        return false;

    return d->m_superHotModifier->held();
}

bool
ShellManager::hudActive() const
{
    if (d->m_hudLoader == NULL) // We are just initializing
        return false;

    return d->m_hudLoader->property("active").toBool();
}

void
ShellManager::setHudActive(bool active)
{
    d->m_hudLoader->setProperty("active", active);
}

QObject *
ShellManager::hudShell() const
{
    return d->m_shellWithHud;
}

int
ShellManager::hudScreen() const
{
    return d->m_shellWithHud->screen()->screen();
}

/*------------------ Hotkeys Handling -----------------------*/

void
ShellManager::onAltF1Pressed()
{
    ShellDeclarativeView * activeShell = d->activeShell();
    if (activeShell) {
        if (dashActive()) {
            // focus the launcher instead of the dash
            setDashActive(false);
            Q_EMIT activeShell->launcherFocusRequested();
        }
        else
        {
            activeShell->toggleLauncher();
        }
    }
}

void
ShellManager::onAltF2Pressed()
{
    ShellDeclarativeView * activeShell = d->activeShell();
    if (activeShell) {
        d->moveDashToShell(activeShell);
        Q_EMIT dashActivateLens(COMMANDS_LENS_ID);
    }
}

void
ShellManager::onNumericHotkeyPressed()
{
    Hotkey* hotkey = qobject_cast<Hotkey*>(sender());
    if (hotkey) {
        ShellDeclarativeView * activeShell = d->activeShell();
        if (activeShell) {
            /* Shortcuts from 1 to 9 should activate the items with index
            from 1 to 9 (index 0 being the so-called "BFB" or Dash launcher).
            Shortcut for 0 should activate item with index 10.
            In other words, the indexes are activated in the same order as
            the keys appear on a standard keyboard. */
            Qt::Key key = hotkey->key();
            if (key >= Qt::Key_1 && key <= Qt::Key_9) {
                int index = key - Qt::Key_0;
                if (hotkey->modifiers() & Qt::ShiftModifier) {
                    Q_EMIT activeShell->newInstanceShortcutPressed(index);
                } else {
                    Q_EMIT activeShell->activateShortcutPressed(index);
                }
            } else if (key == Qt::Key_0) {
                if (hotkey->modifiers() & Qt::ShiftModifier) {
                    Q_EMIT activeShell->newInstanceShortcutPressed(10);
                } else {
                    Q_EMIT activeShell->activateShortcutPressed(10);
                }
            }
        }
    }
}

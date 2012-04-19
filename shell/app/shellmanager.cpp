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
#include <QX11Info>

// libunity-2d-private
#include <debug_p.h>
#include <gkeysequenceparser.h>
#include <hotmodifier.h>
#include <hotkeymonitor.h>
#include <hotkey.h>
#include <keymonitor.h>
#include <screeninfo.h>

// Local
#include "shelldeclarativeview.h"
#include "config.h"

// unity-2d
#include <unity2ddebug.h>
#include <gobjectcallback.h>

// libqtgconf
#include <gconfitem-qml-wrapper.h>

// bamf
#include "bamf-window.h"
#include "bamf-matcher.h"

// libwnck
extern "C" {
#define WNCK_I_KNOW_THIS_IS_UNSTABLE
#include <libwnck/libwnck.h>
}

static const char* COMMANDS_LENS_ID = "commands.lens";
static const int DASH_MIN_SCREEN_WIDTH = 1280;
static const int DASH_MIN_SCREEN_HEIGHT = 1084;
static const char* HUD_SHORTCUT_KEY = "/apps/compiz-1/plugins/unityshell/screen0/options/show_hud";

GOBJECT_CALLBACK1(activeWorkspaceChangedCB, "onActiveWorkspaceChanged");

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
        , m_last_focused_window(None)
    {}

    enum ActiveShellUsage {
        ActiveShellGeneralUse,
        ActiveShellLauncherRelatedUse
    };

    ShellDeclarativeView* initShell(int screen);
    void updateScreenCount(int newCount);
    ShellDeclarativeView* activeShell(ActiveShellUsage usage) const;
    void moveDashToShell(ShellDeclarativeView* newShell);
    void moveHudToShell(ShellDeclarativeView* newShell);
    void saveActiveWindow();

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

    // Only one of the two is non null at a time
    HotModifier* m_hudHotModifier;
    Hotkey* m_hudHotKey;

    WId m_last_focused_window;

    GConfItemQmlWrapper *m_gconfItem;
};


ShellDeclarativeView *
ShellManagerPrivate::initShell(int screen)
{
    const QStringList arguments = qApp->arguments();
    ShellDeclarativeView * view = new ShellDeclarativeView(q, m_sourceFileUrl, screen);
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
ShellManagerPrivate::activeShell(ActiveShellUsage usage) const
{
    bool launcherOnlyInOneScreen = launcher2dConfiguration().property("onlyOneLauncher").toBool();
    if (usage == ActiveShellLauncherRelatedUse && launcherOnlyInOneScreen) {
        return m_viewList.isEmpty() ? NULL : m_viewList[0];
    }

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

    /* Instantiate new Shells as needed. */
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
            if (m_hudLoader != NULL) {
                QObject::connect(m_hudLoader, SIGNAL(activeChanged()), q, SIGNAL(hudActiveChanged()));
            } else {
                qWarning() << "Could not find the hudLoader";
            }
        }
    }

    /* Remove extra Shells if any. */
    while (m_viewList.size() > newCount) {
        ShellDeclarativeView *shell = m_viewList.takeLast();
        if (shell == m_shellWithDash) {
            if (newCount > 0) {
                moveDashToShell(m_viewList[0]);
            } else {
                m_shellWithDash = NULL;
                Q_EMIT q->dashShellChanged(NULL);
                Q_EMIT q->dashScreenChanged(-1);
            }
        }
        if (shell == m_shellWithHud) {
            if (newCount > 0) {
                moveHudToShell(m_viewList[0]);
            } else {
                m_shellWithHud = NULL;
                Q_EMIT q->hudShellChanged(NULL);
                Q_EMIT q->hudScreenChanged(-1);
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

static bool moveRootChildItemToShell(const char *itemName, ShellDeclarativeView* newShell, ShellDeclarativeView* oldShell)
{
    bool itemMoved = false;

    if (newShell != oldShell) {
        QDeclarativeItem *item = qobject_cast<QDeclarativeItem*>(oldShell->rootObject()->property(itemName).value<QObject *>());
        if (item) {
            const QGraphicsView::ViewportUpdateMode oldShellViewportUpdateMode = oldShell->viewportUpdateMode();
            const QGraphicsView::ViewportUpdateMode newShellViewportUpdateMode = newShell->viewportUpdateMode();
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

            oldShell->setViewportUpdateMode(oldShellViewportUpdateMode);
            newShell->setViewportUpdateMode(newShellViewportUpdateMode);

            itemMoved = true;
        } else {
            qWarning() << "moveRootChildItemToShell: Could not find the item" << itemName;
        }
    }

    return itemMoved;
}

void ShellManagerPrivate::moveDashToShell(ShellDeclarativeView* newShell)
{
    if (moveRootChildItemToShell("dashLoader", newShell, m_shellWithDash)) {
        m_shellWithDash = newShell;
        Q_EMIT q->dashShellChanged(newShell);
        Q_EMIT q->dashScreenChanged(q->dashScreen());
    }
}

void ShellManagerPrivate::moveHudToShell(ShellDeclarativeView* newShell)
{
    if (moveRootChildItemToShell("hudLoader", newShell, m_shellWithHud)) {
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
    d->m_hudHotModifier = NULL;
    d->m_hudHotKey = NULL;

    d->m_gconfItem = new GConfItemQmlWrapper(this);
    connect(d->m_gconfItem, SIGNAL(valueChanged()), this, SLOT(onHudActivationShortcutChanged()));
    d->m_gconfItem->setKey(HUD_SHORTCUT_KEY);
    onHudActivationShortcutChanged();

    qmlRegisterUncreatableType<ShellDeclarativeView>("Unity2d", 1, 0, "ShellDeclarativeView", "This can only be created from C++");
    qmlRegisterUncreatableType<ShellManager>("Unity2d", 1, 0, "ShellManager", "This can only be created from C++");

    QDesktopWidget* desktop = QApplication::desktop();

    d->updateScreenCount(desktop->screenCount());

    connect(desktop, SIGNAL(screenCountChanged(int)), SLOT(onScreenCountChanged(int)));

    connect(&launcher2dConfiguration(), SIGNAL(superKeyEnableChanged(bool)), SLOT(updateSuperKeyMonitoring()));
    updateSuperKeyMonitoring();

    /* Super tap shows the dash, super held shows the launcher hints */
    d->m_superHotModifier = KeyMonitor::instance()->getHotModifierFor(Qt::MetaModifier);
    connect(d->m_superHotModifier, SIGNAL(tapped()), SLOT(toggleDashRequested()));
    connect(d->m_superHotModifier, SIGNAL(heldChanged(bool)), SIGNAL(superKeyHeldChanged(bool)));

    /* Alt+F1 reveals the launcher and gives the keyboard focus to the Dash Button. */
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
        hotkey = HotkeyMonitor::instance().getHotkeyFor(key, Qt::MetaModifier | Qt::KeypadModifier);
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

    g_signal_connect(G_OBJECT(wnck_screen_get_default()), "active_workspace_changed", G_CALLBACK(activeWorkspaceChangedCB), this);
}

ShellManager::~ShellManager()
{
    g_signal_handlers_disconnect_by_func(G_OBJECT(wnck_screen_get_default()), gpointer(activeWorkspaceChangedCB), this);

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
    if (d->m_shellWithDash != NULL) {
        return d->m_shellWithDash->screen()->screen();
    } else {
        return -1;
    }
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
ShellManager::toggleDashRequested()
{
    ShellDeclarativeView * activeShell = d->activeShell(ShellManagerPrivate::ActiveShellLauncherRelatedUse);
    if (activeShell) {
        const bool differentShell = d->m_shellWithDash != activeShell;

        if (dashActive() && !differentShell) {
            setDashActive(false);
            forceDeactivateShell(d->m_shellWithDash);
        } else {
            d->moveDashToShell(activeShell);
            Q_EMIT dashActivateHome();
        }
    }
}

void
ShellManager::toggleHudRequested()
{
    ShellDeclarativeView * activeShell = d->activeShell(ShellManagerPrivate::ActiveShellGeneralUse);
    if (activeShell) {
        const bool differentShell = d->m_shellWithHud != activeShell;

        if (differentShell) {
            d->moveHudToShell(activeShell);

            if (!hudActive()) {
                Q_EMIT toggleHud();
            } else {
                forceActivateShell(activeShell);
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

void ShellManager::onActiveWorkspaceChanged()
{
    Q_EMIT activeWorkspaceChanged();
    d->m_last_focused_window = None;
    Q_EMIT lastFocusedWindowChanged(d->m_last_focused_window);
}

void ShellManager::onHudActivationShortcutChanged()
{
    // TODO It might make sense to abstract this logic
    // of switching between hotkey and hotmodifier and put it
    // in a class in libunity-2d-private
    if (d->m_hudHotModifier) {
        d->m_hudHotModifier->disconnect(this);
        d->m_hudHotModifier = NULL;
    }
    if (d->m_hudHotKey) {
        d->m_hudHotKey->disconnect(this);
        d->m_hudHotKey = NULL;
    }

    int x11KeyCode;
    Qt::KeyboardModifiers modifiers;
    const QString shortcut = d->m_gconfItem->getValue().toString();
    const bool success = GKeySequenceParser::parse(shortcut, &x11KeyCode, &modifiers);
    if (success) {
        if (x11KeyCode != 0) {
            d->m_hudHotKey = HotkeyMonitor::instance().getHotkeyFor(x11KeyCode, modifiers);
            connect(d->m_hudHotKey, SIGNAL(pressed()), SLOT(toggleHudRequested()));
        } else if (modifiers != Qt::NoModifier) {
            d->m_hudHotModifier = KeyMonitor::instance()->getHotModifierFor(modifiers);
            connect(d->m_hudHotModifier, SIGNAL(tapped()), SLOT(toggleHudRequested()));
        }
    } else {
        qWarning().nospace() << "Could not parse the key sequence " << shortcut << ". HUD won't be activated";
    }
}

/* ----------------- super key handling ---------------- */

void
ShellManager::updateSuperKeyMonitoring()
{
    KeyMonitor *modifiersMonitor = KeyMonitor::instance();
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
    if (d->m_superHotModifier == NULL) { // We are just initializing
        return false;
    }

    return d->m_superHotModifier->held();
}

bool
ShellManager::hudActive() const
{
    if (d->m_hudLoader == NULL) { // We are just initializing
        return false;
    }

    return d->m_hudLoader->property("active").toBool();
}

void
ShellManager::setHudActive(bool active)
{
    if (d->m_hudLoader != NULL) {
        d->m_hudLoader->setProperty("active", active);
    }
}

QObject *
ShellManager::hudShell() const
{
    return d->m_shellWithHud;
}

int
ShellManager::hudScreen() const
{
    if (d->m_shellWithHud != NULL) {
        return d->m_shellWithHud->screen()->screen();
    } else {
        return -1;
    }
}

/*------------------ Hotkeys Handling -----------------------*/

void
ShellManager::onAltF1Pressed()
{
    ShellDeclarativeView * activeShell = d->activeShell(ShellManagerPrivate::ActiveShellLauncherRelatedUse);
    if (activeShell) {
        if (dashActive()) {
            // focus the launcher instead of the dash
            setDashActive(false);
            Q_EMIT activeShell->launcherFocusRequested();
        } else {
            activeShell->toggleLauncher();
        }
    }
}

void
ShellManager::onAltF2Pressed()
{
    ShellDeclarativeView * activeShell = d->activeShell(ShellManagerPrivate::ActiveShellLauncherRelatedUse);
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
        ShellDeclarativeView * activeShell = d->activeShell(ShellManagerPrivate::ActiveShellLauncherRelatedUse);
        if (activeShell) {
            /* Shortcuts from 1 to 9 should activate the items with index
            from 1 to 9 (index 0 being the so-called "BFB" or Dash launcher).
            Shortcut for 0 should activate item with index 10.
            In other words, the indexes are activated in the same order as
            the keys appear on a standard keyboard. */
            const int key = hotkey->key();
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

unsigned int ShellManager::lastFocusedWindow() const
{
    return d->m_last_focused_window;
}

/* Obtaining & Discarding Keyboard Focus for Window on Demand
 *
 * In the X world, activating a window means to give it the input (keyboard)
 * focus. When a new window opens, X usually makes it active immediately.
 * Clicking on a window makes it active too.
 *
 * Qt does not have the capability to explicitly ask the window manager to
 * make an existing window active - setFocus() only forwards input focus to
 * whatever QWidget you specify.
 *
 * De-Activating a window is not possible with X (and hence with Qt). So
 * we work-around this by remembering which application is active prior to
 * stealing focus, and then Re-Activating it when we're finished. This is
 * not guaranteed to succeed, as previous window may have closed.
 *
 * The following methods deal with these tasks. Note that when the window
 * has been activated (deactivated), Qt will realise it has obtained (lost)
 * focus and act appropriately.
 */

/* Save WId of window with keyboard focus to m_last_focused_window */
void ShellManagerPrivate::saveActiveWindow()
{
    /* Using Bamf here, 'cause XGetFocusInputFocus returned a XId
       different by 1, which then could not be used with Bamf to
       get the application. The change does not result in any functional
       differences, though. */
    const BamfWindow* bamf_active_window = BamfMatcher::get_default().active_window();

    /* Bamf can return a null active window - example case is just after 
       login when no application has been yet been started. */
    const WId active_window = bamf_active_window != NULL ? bamf_active_window->xid() : None;

    bool notAShell = true;
    Q_FOREACH(ShellDeclarativeView * shell, m_viewList) {
        notAShell = notAShell && active_window != shell->effectiveWinId();
    }

    if (notAShell && active_window != m_last_focused_window) {
        m_last_focused_window = active_window;
        Q_EMIT q->lastFocusedWindowChanged(m_last_focused_window);
    }
}

/* Ask Window Manager to activate this window and hence get keyboard focus */
void ShellManager::forceActivateShell(ShellDeclarativeView *shell)
{
    // Save reference to window with current keyboard focus
    if( d->m_last_focused_window == None ){
        d->saveActiveWindow();
    }

    // Show this window by giving it keyboard focus
    Unity2DDeclarativeView::forceActivateWindow(shell->effectiveWinId(), shell);
}

/* Ask Window Manager to deactivate this window - not guaranteed to succeed. */
void ShellManager::forceDeactivateShell(ShellDeclarativeView *shell)
{
    if( d->m_last_focused_window == None ){
        UQ_WARNING << "No previously focused window found, use mouse to select window.";
        return;
    }

    // What if previously focused window closed while we we had focus? Check if window
    // exists by seeing if it has attributes.
    XWindowAttributes attributes;
    const int status = XGetWindowAttributes(QX11Info::display(), d->m_last_focused_window, &attributes);
    if ( status == BadWindow ){
        UQ_WARNING << "Previously focused window has gone, use mouse to select window.";
        return;
    }

    // Show this window by giving it keyboard focus
    Unity2DDeclarativeView::forceActivateWindow(d->m_last_focused_window);

    d->m_last_focused_window = None;
    Q_EMIT lastFocusedWindowChanged(d->m_last_focused_window);
}

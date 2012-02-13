/*
 * This file is part of unity-2d
 *
 * Copyright 2010 Canonical Ltd.
 *
 * Authors:
 * - Olivier Tilloy <olivier.tilloy@canonical.com>
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

// Self
#include "panelmanager.h"

// Local
#include <config.h>
#include <panelstyle.h>
#include <indicatorsmanager.h>
#include <keymonitor.h>
#include <keyboardmodifiersmonitor.h>
#include <hotkeymonitor.h>
#include <hotkey.h>
#include <debug_p.h>

// Unity
#include <unity2dpanel.h>
#include <panelappletproviderinterface.h>

// Qt
#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QDir>
#include <QFileInfo>
#include <QHash>
#include <QPluginLoader>
#include <QProcessEnvironment>
#include <QVariant>
#include <QtDBus/QDBusInterface>
#include <QtDBus/QDBusPendingCall>
#include <QtDBus/QDBusReply>
#include <QtDBus/QDBusConnectionInterface>

using namespace Unity2d;

static const char* PANEL_DCONF_PROPERTY_APPLETS = "applets";
static const char* PANEL_PLUGINS_DEV_DIR_ENV = "UNITY2D_PANEL_PLUGINS_PATH";

static const char* HUD_DBUS_SERVICE = "com.canonical.Unity2d.Hud";
static const char* HUD_DBUS_PATH = "/Hud";
static const char* HUD_DBUS_INTERFACE = "com.canonical.Unity2d.Hud";
static const char* HUD_DBUS_PROPERTY_ACTIVE = "active";

static const char* SPREAD_DBUS_SERVICE = "com.canonical.Unity2d.Spread";
static const char* SPREAD_DBUS_PATH = "/Spread";
static const char* SPREAD_DBUS_INTERFACE = "com.canonical.Unity2d.Spread";
static const char* SPREAD_DBUS_METHOD_IS_SHOWN = "IsShown";

static const int KEY_HOLD_THRESHOLD = 250;

static QHash<QString, PanelAppletProviderInterface*> loadPlugins()
{
    QHash<QString, PanelAppletProviderInterface*> plugins;

    /* When running uninstalled the plugins will be loaded from the source tree
       under panel/applets.
       When running installed the plugins will be typically in /usr/lib/unity-2d/plugins/panel
       In both cases it's still possible to override these default locations by setting an
       environment variable with the path where the plugins are located.
    */

    QString panelPluginsDefaultDir = unity2dPluginsPath() + "/panel";
    if (!isRunningInstalled()) panelPluginsDefaultDir += "/applets";
    QString pluginPath = QProcessEnvironment::systemEnvironment().value(PANEL_PLUGINS_DEV_DIR_ENV,
                                                                        panelPluginsDefaultDir);
    QFileInfo pluginFileInfo = QFileInfo(pluginPath);
    if (!pluginFileInfo.isDir()) {
        qWarning() << "Panel plugin directory does not exist:" << pluginPath;
        return plugins;
    }

    qDebug() << "Scanning panel plugin directory" << pluginFileInfo.absoluteFilePath();

    QDir pluginDir = QDir(pluginFileInfo.absoluteFilePath());
    QStringList filters;
    filters << "*.so";
    pluginDir.setNameFilters(filters);

    Q_FOREACH(const QString& fileEntry, pluginDir.entryList()) {
        QString pluginFilePath = pluginDir.absoluteFilePath(fileEntry);
        qDebug() << "Loading panel plugin:" << pluginFilePath;

        QPluginLoader loader(pluginFilePath);
        if (loader.load()) {
            PanelAppletProviderInterface* provider;
            provider = qobject_cast<PanelAppletProviderInterface*>(loader.instance());
            if (provider == 0) {
                qWarning() << "Plugin loaded from" << pluginFilePath
                           << "does not implement the interface PanelAppletProviderInterface";
            } else {
               plugins.insert(provider->appletName(), provider);
            }
        } else {
            qWarning() << "Failed to load panel plugin from" << pluginFilePath
                       << "with error" << loader.errorString();
        }
    }

    return plugins;
}

QStringList PanelManager::loadPanelConfiguration() const
{
    QVariant appletsConfig = panel2dConfiguration().property(PANEL_DCONF_PROPERTY_APPLETS);
    if (!appletsConfig.isValid()) {
        qWarning() << "Missing or invalid panel applets configuration in dconf. Please check"
                   << "the property" << PANEL_DCONF_PROPERTY_APPLETS
                   << "in schema" << PANEL2D_DCONF_SCHEMA;
        return QStringList();
    }

    return appletsConfig.toStringList();
}

PanelManager::PanelManager(QObject* parent)
: QObject(parent)
, m_altKeyPressed(false)
, m_altKeyHeld(false)
{
    Unity2dPanel* panel;
    QDesktopWidget* desktop = QApplication::desktop();

    QPoint p;
    if (QApplication::isRightToLeft()) {
        p = QPoint(desktop->width() - 1, 0);
    }
    int leftmost = desktop->screenNumber(p);

    panel = instantiatePanel(leftmost);
    m_panels.append(panel);
    panel->show();
    panel->move(desktop->screenGeometry(leftmost).topLeft());

    for(int i = 0; i < desktop->screenCount(); ++i) {
        if (i == leftmost) {
            continue;
        }
        panel = instantiatePanel(i);
        m_panels.append(panel);
        panel->show();
        panel->move(desktop->screenGeometry(i).topLeft());
    }
    connect(desktop, SIGNAL(screenCountChanged(int)), SLOT(updateScreenLayout(int)));
    connect(desktop, SIGNAL(resized(int)), SLOT(onScreenResized(int)));

    /* Alt key hold timer */
    m_altKeyHoldTimer.setSingleShot(true);
    m_altKeyHoldTimer.setInterval(KEY_HOLD_THRESHOLD);
    connect(&m_altKeyHoldTimer, SIGNAL(timeout()), SLOT(updateAltKeyHoldState()));

    connect(KeyboardModifiersMonitor::instance(),
            SIGNAL(keyboardModifiersChanged(Qt::KeyboardModifiers)),
            SLOT(onKeyboardModifiersChanged(Qt::KeyboardModifiers)));

    /* A F10 keypress opens the first menu of the visible application or of the first
       indicator on the panel */
    Hotkey* F10 = HotkeyMonitor::instance().getHotkeyFor(Qt::Key_F10, Qt::AltModifier);
    connect(F10, SIGNAL(released()), SLOT(onAltF10Pressed()));
}

PanelManager::~PanelManager()
{
    qDeleteAll(m_panels);
}

Unity2dPanel* PanelManager::instantiatePanel(int screen)
{
    Unity2dPanel* panel = new Unity2dPanel(false, screen);
    panel->setAccessibleName("Top Panel");
    panel->setEdge(Unity2dPanel::TopEdge);
    panel->setFixedHeight(24);

    QPoint p;
    if (QApplication::isRightToLeft()) {
        p = QPoint(QApplication::desktop()->width() - 1, 0);
    }
    int leftmost = QApplication::desktop()->screenNumber(p);

    QHash<QString, PanelAppletProviderInterface*> plugins = loadPlugins();

    QStringList panelConfiguration = loadPanelConfiguration();
    qDebug() << "Configured plugins list is:" << panelConfiguration;

    Q_FOREACH(QString appletName, panelConfiguration) {
        bool onlyLeftmost = appletName.startsWith('!');
        if (onlyLeftmost) {
            appletName = appletName.mid(1);
        }

        PanelAppletProviderInterface* provider = plugins.value(appletName, NULL);
        if (provider == 0) {
            qWarning() << "Panel applet" << appletName << "was requested but there's no"
                       << "installed plugin providing it.";
        } else {
            if (screen == leftmost || !onlyLeftmost) {
                QWidget* applet = provider->createApplet(panel);
                if (applet == 0) {
                    qWarning() << "The panel applet plugin for" << appletName
                               << "did not return a valid plugin.";
                } else {
                    panel->addWidget(applet);
                }
            }
        }
    }

    return panel;
}

void
PanelManager::onScreenResized(int screen)
{
    QPoint p;
    QDesktopWidget* desktop = QApplication::desktop();
    if (QApplication::isRightToLeft()) {
        p = QPoint(desktop->width() - 1, 0);
    }
    int leftmost = desktop->screenNumber(p);

    /*  We only care about the leftmost screen being resized,
        because there is no screenLayoutChanged signal, we're
        abusing it here so that we update the panels  */
    if (screen == leftmost) {
        updateScreenLayout(desktop->screenCount());
    }
}

void
PanelManager::updateScreenLayout(int newCount)
{
    if (newCount > 0) {
        QDesktopWidget* desktop = QApplication::desktop();
        int size = m_panels.size();
        Unity2dPanel* panel;

        /* The first panel is always the one on the leftmost screen. */
        int leftmost = desktop->screenNumber(QPoint());
        if (size > 0) {
            panel = m_panels[0];
        } else {
            panel = instantiatePanel(leftmost);
            m_panels.append(panel);
        }
        panel->show();
        panel->setScreen(leftmost);

        /* Update the position of other existing panels, and instantiate new
           panels as needed. */
        int i = 1;
        for (int screen = 0; screen < newCount; ++screen) {
            if (screen == leftmost) {
                continue;
            }
            if (i < size) {
                panel = m_panels[i];
            } else {
                panel = instantiatePanel(screen);
                m_panels.append(panel);
            }
            panel->show();
            panel->setScreen(screen);
            ++i;
        }
    }
    /* Remove extra panels if any. */
    while (m_panels.size() > newCount) {
        delete m_panels.takeLast();
    }
}

void PanelManager::onAltF10Pressed()
{
    QDesktopWidget* desktop = QApplication::desktop();
    int screen = desktop->screenNumber(QCursor::pos());
    Unity2dPanel* panel;
    
    if (screen >= m_panels.size()) {
        return;
    }
    panel = m_panels[screen];
    if (panel != NULL) {
        QEvent* event = new QEvent(Unity2dPanel::SHOW_FIRST_MENU_EVENT);
        QCoreApplication::postEvent(panel, event);
    }
}

void PanelManager::toggleHud()
{
    QDBusInterface hudInterface(HUD_DBUS_SERVICE, HUD_DBUS_PATH, HUD_DBUS_INTERFACE);

    QVariant hudActiveResult = hudInterface.property(HUD_DBUS_PROPERTY_ACTIVE);
    if (!hudActiveResult.isValid()) {
        UQ_WARNING << "Can't read the DBUS Hud property" << HUD_DBUS_PROPERTY_ACTIVE
                   << "on" << HUD_DBUS_SERVICE << HUD_DBUS_PATH << HUD_DBUS_INTERFACE;
        return;
    }

    bool hudActive = hudActiveResult.toBool();

    if (hudActive) {
        /* Check if the spread is active before activating the hud.
           We need to do this since the spread can't prevent the shell from
           monitoring the alt key and therefore getting to this point if
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
    }

    if (!hudInterface.setProperty(HUD_DBUS_PROPERTY_ACTIVE, !hudActive)) {
        UQ_WARNING << "Can't set the DBUS Hud property" << HUD_DBUS_PROPERTY_ACTIVE
                   << "on" << HUD_DBUS_SERVICE << HUD_DBUS_PATH << HUD_DBUS_INTERFACE;
    }
}

/* ----------------- alt key handling ---------------- */

void PanelManager::onKeyboardModifiersChanged(Qt::KeyboardModifiers modifiers)
{
    KeyMonitor *keyMonitor = KeyMonitor::instance();

    /* This is the new new state of the Alt key, while
       m_altKeyPressed is the previous state of the key at the last modifiers change. */
    bool altKeyPressed = modifiers.testFlag(Qt::AltModifier);

    if (m_altKeyPressed != altKeyPressed) {
        m_altKeyPressed = altKeyPressed;
        if (altKeyPressed) {
            m_altPressIgnored = false;
            /* If any other key is pressed with Alt, make sure we detect it so an Alt-tap
               isn't registered */
            connect(keyMonitor, SIGNAL(keyPressed()),
                    this, SLOT(ignoreAltPress()));

            /* If the key is pressed, start up a timer to monitor if it's being held short
               enough to qualify as just a "tap" or as a proper hold */
            m_altKeyHoldTimer.start();
        } else {
            m_altKeyHoldTimer.stop();
            /* Now Alt is released, can stop watching for other keys */
            disconnect(keyMonitor, SIGNAL(keyPressed()),
                       this, SLOT(ignoreAltPress()));

            /* If the key is released, and was not being held, it means that the user just
               performed a "tap". Unless we're told to ignore that tap, that is. */
            if (!m_altKeyHeld && !m_altPressIgnored) {
                toggleHud();
            }
            /* Otherwise the user just terminated a hold. */
            else if(m_altKeyHeld){
                m_altKeyHeld = false;
                Q_EMIT altKeyHeldChanged(m_altKeyHeld);
                /* TODO: start fading out the panel menubar */
            }
        }
    }
}

void PanelManager::updateAltKeyHoldState()
{
    /* If the key was released in the meantime, just do nothing, otherwise
       consider the key being held, unless we're told to ignore it. */
    if (m_altKeyPressed && !m_altPressIgnored) {
        m_altKeyHeld = true;
        Q_EMIT altKeyHeldChanged(m_altKeyHeld);
    } else {
        /* TODO: start fading in the panel menubar */
    }
}

void PanelManager::ignoreAltPress()
{
    /* There was a key pressed, ignore current alt tap/hold */
    m_altPressIgnored = true;
    /* TODO: immediately show panel menubar*/
}

#include "panelmanager.moc"


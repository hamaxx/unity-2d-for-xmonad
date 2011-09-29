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
#include <hotkeymonitor.h>
#include <hotkey.h>

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

using namespace Unity2d;

static const char* PANEL_DCONF_SCHEMA = "com.canonical.Unity2d.Panel";
static const char* PANEL_DCONF_PROPERTY_APPLETS = "applets";
static const char* PANEL_PLUGINS_DEV_DIR_ENV = "UNITY2D_PANEL_PLUGINS_PATH";

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

    Q_FOREACH(QString fileEntry, pluginDir.entryList()) {
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
    QVariant appletsConfig = m_conf.property(PANEL_DCONF_PROPERTY_APPLETS);
    if (!appletsConfig.isValid()) {
        qWarning() << "Missing or invalid panel applets configuration in dconf. Please check"
                   << "the property" << PANEL_DCONF_PROPERTY_APPLETS
                   << "in schema" << PANEL_DCONF_SCHEMA;
        return QStringList();
    }

    return appletsConfig.toStringList();
}

PanelManager::PanelManager(QObject* parent)
: QObject(parent)
, m_conf(PANEL_DCONF_SCHEMA)
{
    QDesktopWidget* desktop = QApplication::desktop();
    for(int i = 0; i < desktop->screenCount(); ++i) {
        Unity2dPanel* panel = instantiatePanel(i);
        m_panels.append(panel);
        panel->show();
        panel->move(desktop->screenGeometry(i).topLeft());
    }
    connect(desktop, SIGNAL(screenCountChanged(int)), SLOT(onScreenCountChanged(int)));

    /* A F10 keypress opens the first menu of the visible application or of the first
       indicator on the panel */
    Hotkey* F10 = HotkeyMonitor::instance().getHotkeyFor(Qt::Key_F10, Qt::NoModifier);
    connect(F10, SIGNAL(released()), SLOT(onF10Pressed()));
}

PanelManager::~PanelManager()
{
    qDeleteAll(m_panels);
}

Unity2dPanel* PanelManager::instantiatePanel(int screen)
{
    Unity2dPanel* panel = new Unity2dPanel;
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
PanelManager::onScreenCountChanged(int newCount)
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
        panel->move(desktop->screenGeometry(leftmost).topLeft());

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
            panel->move(desktop->screenGeometry(screen).topLeft());
            ++i;
        }
    }
    /* Remove extra panels if any. */
    while (m_panels.size() > newCount) {
        delete m_panels.takeLast();
    }
}

void PanelManager::onF10Pressed()
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

#include "panelmanager.moc"

